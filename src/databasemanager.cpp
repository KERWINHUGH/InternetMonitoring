#include "databasemanager.h"
#include <QDir>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDriver>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), connected(false)
{
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
        emit databaseDisconnected();
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initDatabase()
{
    if (connected) return true;

    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QDir::currentPath() + "/internetmonitoring.db";
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        setLastError("无法打开数据库: " + db.lastError().text());
        return false;
    }

    connected = true;
    emit databaseConnected();
    return createTables();
}

void DatabaseManager::setLastError(const QString& error)
{
    lastErrorMsg = error;
    emit databaseError(error);
    qDebug() << "数据库错误:" << error;
}

bool DatabaseManager::beginTransaction()
{
    if (!connected) {
        setLastError("数据库未连接");
        return false;
    }
    return db.transaction();
}

bool DatabaseManager::commitTransaction()
{
    if (!connected) {
        setLastError("数据库未连接");
        return false;
    }
    return db.commit();
}

bool DatabaseManager::rollbackTransaction()
{
    if (!connected) {
        setLastError("数据库未连接");
        return false;
    }
    return db.rollback();
}

bool DatabaseManager::createTables()
{
    // 创建用户表
    if (!executeQuery("CREATE TABLE IF NOT EXISTS users ("
                     "username TEXT PRIMARY KEY,"
                     "password TEXT NOT NULL,"
                     "email TEXT,"
                     "phone TEXT,"
                     "nickname TEXT,"
                     "role TEXT NOT NULL"
                     ")")) {
        return false;
    }

    // 创建设备表
    if (!executeQuery("CREATE TABLE IF NOT EXISTS devices ("
                     "device_id TEXT PRIMARY KEY,"
                     "name TEXT NOT NULL,"
                     "type TEXT NOT NULL,"
                     "location TEXT,"
                     "status TEXT DEFAULT 'offline'"
                     ")")) {
        return false;
    }

    // 创建监控数据表
    if (!executeQuery("CREATE TABLE IF NOT EXISTS monitor_data ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "device_id TEXT NOT NULL,"
                     "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                     "temperature REAL,"
                     "humidity REAL,"
                     "cpu_usage REAL,"
                     "memory_usage REAL,"
                     "network_speed REAL,"
                     "FOREIGN KEY(device_id) REFERENCES devices(device_id)"
                     ")")) {
        return false;
    }

    // 创建告警表
    if (!executeQuery("CREATE TABLE IF NOT EXISTS alarms ("
                     "alarm_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "device_id TEXT NOT NULL,"
                     "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                     "type TEXT NOT NULL,"
                     "description TEXT NOT NULL,"
                     "level TEXT NOT NULL,"
                     "status TEXT DEFAULT 'unhandled',"
                     "FOREIGN KEY(device_id) REFERENCES devices(device_id)"
                     ")")) {
        return false;
    }

    // 创建系统日志表
    if (!executeQuery("CREATE TABLE IF NOT EXISTS system_logs ("
                     "log_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                     "type TEXT NOT NULL,"
                     "content TEXT NOT NULL,"
                     "username TEXT NOT NULL"
                     ")")) {
        return false;
    }

    return true;
}

bool DatabaseManager::executeQuery(const QString& sql)
{
    if (!connected) {
        setLastError("数据库未连接");
        return false;
    }

    QSqlQuery query;
    if (!query.exec(sql)) {
        setLastError("SQL执行失败: " + query.lastError().text() + "\nSQL语句: " + sql);
        return false;
    }
    return true;
}

// 用户管理方法实现
bool DatabaseManager::addUser(const QString& username, const QString& password,
                            const QString& email, const QString& phone,
                            const QString& nickname, const QString& role)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, email, phone, nickname, role) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
    
    // 使用SHA-256加密密码
    QByteArray hashedPassword = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256).toHex();
    
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(nickname);
    query.addBindValue(role);

    if (!query.exec()) {
        qDebug() << "添加用户失败:" << query.lastError().text();
        return false;
    }

    // 添加系统日志
    addLog("用户管理", QString("新用户 %1 已创建").arg(username), "system");
    return true;
}

bool DatabaseManager::updateUser(const QString& username, const QString& email,
                               const QString& phone, const QString& nickname)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET email=?, phone=?, nickname=? WHERE username=?");
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(nickname);
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "更新用户信息失败:" << query.lastError().text();
        return false;
    }

    addLog("用户管理", QString("用户 %1 信息已更新").arg(username), username);
    return true;
}

bool DatabaseManager::updatePassword(const QString& username, const QString& newPassword)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET password=? WHERE username=?");
    
    QByteArray hashedPassword = QCryptographicHash::hash(
        newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    
    query.addBindValue(hashedPassword);
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "更新密码失败:" << query.lastError().text();
        return false;
    }

    addLog("用户管理", QString("用户 %1 密码已更新").arg(username), username);
    return true;
}

bool DatabaseManager::deleteUser(const QString& username)
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE username=?");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "删除用户失败:" << query.lastError().text();
        return false;
    }

    addLog("用户管理", QString("用户 %1 已删除").arg(username), "system");
    return true;
}

bool DatabaseManager::verifyUser(const QString& username, const QString& password, QString& role)
{
    QSqlQuery query;
    query.prepare("SELECT password, role FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        return false;
    }

    QByteArray hashedPassword = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256).toHex();
    
    if (query.value(0).toString() == hashedPassword) {
        role = query.value(1).toString();
        addLog("用户登录", QString("用户 %1 登录成功").arg(username), username);
        return true;
    }

    return false;
}

bool DatabaseManager::getUserInfo(const QString& username, QString& email,
                                QString& phone, QString& nickname, QString& role)
{
    QSqlQuery query;
    query.prepare("SELECT email, phone, nickname, role FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        return false;
    }

    email = query.value(0).toString();
    phone = query.value(1).toString();
    nickname = query.value(2).toString();
    role = query.value(3).toString();
    return true;
}

// 设备管理方法实现
bool DatabaseManager::addDevice(const QString& deviceId, const QString& name,
                              const QString& type, const QString& location)
{
    QSqlQuery query;
    query.prepare("INSERT INTO devices (device_id, name, type, location) "
                 "VALUES (?, ?, ?, ?)");
    query.addBindValue(deviceId);
    query.addBindValue(name);
    query.addBindValue(type);
    query.addBindValue(location);

    if (!query.exec()) {
        qDebug() << "添加设备失败:" << query.lastError().text();
        return false;
    }

    addLog("设备管理", QString("新设备 %1 已添加").arg(deviceId), "system");
    return true;
}

bool DatabaseManager::updateDevice(const QString& deviceId, const QString& name,
                                 const QString& type, const QString& location)
{
    QSqlQuery query;
    query.prepare("UPDATE devices SET name=?, type=?, location=? WHERE device_id=?");
    query.addBindValue(name);
    query.addBindValue(type);
    query.addBindValue(location);
    query.addBindValue(deviceId);

    if (!query.exec()) {
        qDebug() << "更新设备失败:" << query.lastError().text();
        return false;
    }

    addLog("设备管理", QString("设备 %1 信息已更新").arg(deviceId), "system");
    return true;
}

bool DatabaseManager::deleteDevice(const QString& deviceId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM devices WHERE device_id=?");
    query.addBindValue(deviceId);

    if (!query.exec()) {
        qDebug() << "删除设备失败:" << query.lastError().text();
        return false;
    }

    addLog("设备管理", QString("设备 %1 已删除").arg(deviceId), "system");
    return true;
}

QVariantList DatabaseManager::getDevices()
{
    QVariantList devices;
    QSqlQuery query("SELECT device_id, name, type, location, status FROM devices");

    while (query.next()) {
        QVariantMap device;
        device["device_id"] = query.value(0).toString();
        device["name"] = query.value(1).toString();
        device["type"] = query.value(2).toString();
        device["location"] = query.value(3).toString();
        device["status"] = query.value(4).toString();
        devices.append(device);
    }

    return devices;
}

// 监控数据方法实现
bool DatabaseManager::addMonitorData(const QString& deviceId, double temperature,
                                   double humidity, double cpuUsage,
                                   double memoryUsage, double networkSpeed)
{
    QSqlQuery query;
    query.prepare("INSERT INTO monitor_data (device_id, temperature, humidity, "
                 "cpu_usage, memory_usage, network_speed) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(deviceId);
    query.addBindValue(temperature);
    query.addBindValue(humidity);
    query.addBindValue(cpuUsage);
    query.addBindValue(memoryUsage);
    query.addBindValue(networkSpeed);

    if (!query.exec()) {
        qDebug() << "添加监控数据失败:" << query.lastError().text();
        return false;
    }

    // 更新设备状态
    query.prepare("UPDATE devices SET status='online' WHERE device_id=?");
    query.addBindValue(deviceId);
    query.exec();

    return true;
}

QVariantList DatabaseManager::getDeviceData(const QString& deviceId,
                                          const QDateTime& startTime,
                                          const QDateTime& endTime)
{
    QVariantList dataList;
    QSqlQuery query;
    query.prepare("SELECT timestamp, temperature, humidity, cpu_usage, "
                 "memory_usage, network_speed FROM monitor_data "
                 "WHERE device_id=? AND timestamp BETWEEN ? AND ? "
                 "ORDER BY timestamp DESC");
    query.addBindValue(deviceId);
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap data;
            data["timestamp"] = query.value(0).toDateTime();
            data["temperature"] = query.value(1).toDouble();
            data["humidity"] = query.value(2).toDouble();
            data["cpu_usage"] = query.value(3).toDouble();
            data["memory_usage"] = query.value(4).toDouble();
            data["network_speed"] = query.value(5).toDouble();
            dataList.append(data);
        }
    }

    return dataList;
}

// 告警管理方法实现
bool DatabaseManager::addAlarm(const QString& deviceId, const QString& type,
                             const QString& description, const QString& level)
{
    QSqlQuery query;
    query.prepare("INSERT INTO alarms (device_id, type, description, level) "
                 "VALUES (?, ?, ?, ?)");
    query.addBindValue(deviceId);
    query.addBindValue(type);
    query.addBindValue(description);
    query.addBindValue(level);

    if (!query.exec()) {
        qDebug() << "添加告警失败:" << query.lastError().text();
        return false;
    }

    addLog("告警管理", QString("设备 %1 产生新告警: %2").arg(deviceId, description), "system");
    return true;
}

bool DatabaseManager::updateAlarmStatus(int alarmId, const QString& status)
{
    QSqlQuery query;
    query.prepare("UPDATE alarms SET status=? WHERE alarm_id=?");
    query.addBindValue(status);
    query.addBindValue(alarmId);

    if (!query.exec()) {
        qDebug() << "更新告警状态失败:" << query.lastError().text();
        return false;
    }

    addLog("告警管理", QString("告警 #%1 状态更新为: %2").arg(alarmId).arg(status), "system");
    return true;
}

QVariantList DatabaseManager::getAlarms(const QString& deviceId,
                                      const QDateTime& startTime,
                                      const QDateTime& endTime)
{
    QVariantList alarms;
    QString sql = "SELECT alarm_id, device_id, timestamp, type, description, "
                 "level, status FROM alarms WHERE 1=1";
    QStringList conditions;
    QVariantList params;

    if (!deviceId.isEmpty()) {
        conditions << "device_id=?";
        params << deviceId;
    }
    if (startTime.isValid()) {
        conditions << "timestamp>=?";
        params << startTime;
    }
    if (endTime.isValid()) {
        conditions << "timestamp<=?";
        params << endTime;
    }

    if (!conditions.isEmpty()) {
        sql += " AND " + conditions.join(" AND ");
    }
    sql += " ORDER BY timestamp DESC";

    QSqlQuery query;
    query.prepare(sql);
    for (const QVariant& param : params) {
        query.addBindValue(param);
    }

    if (query.exec()) {
        while (query.next()) {
            QVariantMap alarm;
            alarm["alarm_id"] = query.value(0).toInt();
            alarm["device_id"] = query.value(1).toString();
            alarm["timestamp"] = query.value(2).toDateTime();
            alarm["type"] = query.value(3).toString();
            alarm["description"] = query.value(4).toString();
            alarm["level"] = query.value(5).toString();
            alarm["status"] = query.value(6).toString();
            alarms.append(alarm);
        }
    }

    return alarms;
}

// 系统日志方法实现
bool DatabaseManager::addLog(const QString& type, const QString& content,
                           const QString& username)
{
    QSqlQuery query;
    query.prepare("INSERT INTO system_logs (type, content, username) "
                 "VALUES (?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(content);
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "添加系统日志失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QVariantList DatabaseManager::getLogs(const QDateTime& startTime,
                                    const QDateTime& endTime)
{
    QVariantList logs;
    QString sql = "SELECT log_id, timestamp, type, content, username "
                 "FROM system_logs WHERE 1=1";
    QStringList conditions;
    QVariantList params;

    if (startTime.isValid()) {
        conditions << "timestamp>=?";
        params << startTime;
    }
    if (endTime.isValid()) {
        conditions << "timestamp<=?";
        params << endTime;
    }

    if (!conditions.isEmpty()) {
        sql += " AND " + conditions.join(" AND ");
    }
    sql += " ORDER BY timestamp DESC";

    QSqlQuery query;
    query.prepare(sql);
    for (const QVariant& param : params) {
        query.addBindValue(param);
    }

    if (query.exec()) {
        while (query.next()) {
            QVariantMap log;
            log["log_id"] = query.value(0).toInt();
            log["timestamp"] = query.value(1).toDateTime();
            log["type"] = query.value(2).toString();
            log["content"] = query.value(3).toString();
            log["username"] = query.value(4).toString();
            logs.append(log);
        }
    }

    return logs;
}