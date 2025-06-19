#include "databasemanager.h"
#include <QDir>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDriver>
#include <QFile>

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
    // 移除数据库连接
    QString connectionName = db.connectionName();
    db = QSqlDatabase();  // 释放数据库连接
    QSqlDatabase::removeDatabase(connectionName);  // 从连接池中移除
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::dropTables()
{
    QStringList tables = {"users", "devices", "monitor_data", "alarm_rules", "alarm_records", "system_logs"};
    bool success = true;
    
    for (const QString& table : tables) {
        if (!executeQuery(QString("DROP TABLE IF EXISTS %1").arg(table))) {
            success = false;
        }
    }
    
    return success;
}

bool DatabaseManager::initDatabase()
{
    if (connected) {
        QString connectionName = db.connectionName();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
        connected = false;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QDir::currentPath() + "/internetmonitoring.db";
    qDebug() << "程序实际使用的数据库路径:" << dbPath;
    db.setDatabaseName(dbPath);

    bool dbExists = QFile::exists(dbPath);

    if (!db.open()) {
        setLastError("无法打开数据库: " + db.lastError().text());
        return false;
    }

    connected = true;
    emit databaseConnected();

    if (!dbExists) {
        // 仅首次创建数据库时建表
        if (!createTables()) {
            setLastError("创建表失败");
            return false;
        }
    }
    return true;
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
    // 表结构与SQL脚本保持一致
    return executeQuery("CREATE TABLE users ("
                       "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT UNIQUE NOT NULL,"
                       "password TEXT NOT NULL,"
                       "email TEXT UNIQUE NOT NULL,"
                       "phone TEXT UNIQUE NOT NULL,"
                       "nickname TEXT,"
                       "role TEXT NOT NULL"
                       ")")
        && executeQuery("CREATE TABLE devices ("
                       "device_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "name TEXT NOT NULL,"
                       "type TEXT NOT NULL,"
                       "location TEXT NOT NULL,"
                       "manufacturer TEXT,"
                       "model TEXT,"
                       "installation_date DATE"
                       ")")
        && executeQuery("CREATE TABLE monitor_data ("
                       "data_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "device_id INTEGER NOT NULL,"
                       "timestamp DATETIME NOT NULL,"
                       "temperature REAL,"
                       "humidity REAL,"
                       "light REAL,"
                       "FOREIGN KEY(device_id) REFERENCES devices(device_id)"
                       ")")
        && executeQuery("CREATE TABLE alarm_rules ("
                       "rule_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "device_id INTEGER NOT NULL,"
                       "description TEXT NOT NULL,"
                       "condition TEXT NOT NULL,"
                       "action TEXT NOT NULL,"
                       "FOREIGN KEY(device_id) REFERENCES devices(device_id)"
                       ")")
        && executeQuery("CREATE TABLE alarm_records ("
                       "alarm_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "device_id INTEGER NOT NULL,"
                       "timestamp DATETIME NOT NULL,"
                       "content TEXT NOT NULL,"
                       "status TEXT NOT NULL,"
                       "note TEXT,"
                       "FOREIGN KEY(device_id) REFERENCES devices(device_id)"
                       ")")
        && executeQuery("CREATE TABLE system_logs ("
                       "log_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "timestamp DATETIME NOT NULL,"
                       "log_type TEXT NOT NULL,"
                       "log_level TEXT NOT NULL,"
                       "content TEXT NOT NULL,"
                       "user_id INTEGER,"
                       "device_id INTEGER,"
                       "FOREIGN KEY(user_id) REFERENCES users(user_id),"
                       "FOREIGN KEY(device_id) REFERENCES devices(device_id)"
                       ")");
    qDebug() << "所有表已重建";
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

// 用户管理
bool DatabaseManager::addUser(const QString& username, const QString& password,
                const QString& email, const QString& phone,
                const QString& nickname, const QString& role)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, email, phone, nickname, role) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(nickname);
    query.addBindValue(role);
    if (!query.exec()) {
        setLastError("添加用户失败: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::updateUser(int user_id, const QString& email,
                   const QString& phone, const QString& nickname)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET email=?, phone=?, nickname=? WHERE user_id=?");
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(nickname);
    query.addBindValue(user_id);
    return query.exec();
}

bool DatabaseManager::updatePassword(int user_id, const QString& newPassword)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET password=? WHERE user_id=?");
    QByteArray hashedPassword = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    query.addBindValue(hashedPassword);
    query.addBindValue(user_id);
    return query.exec();
}

bool DatabaseManager::deleteUser(int user_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE user_id=?");
    query.addBindValue(user_id);
    return query.exec();
}

bool DatabaseManager::verifyUser(const QString& username, const QString& password, int& user_id, QString& role)
{
    QSqlQuery query;
    query.prepare("SELECT user_id, password, role FROM users WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec() || !query.next()) {
        return false;
    }
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    if (query.value(1).toString() == hashedPassword) {
        user_id = query.value(0).toInt();
        role = query.value(2).toString();
        return true;
    }
    return false;
}

bool DatabaseManager::getUserInfo(int user_id, QString& username, QString& email,
                    QString& phone, QString& nickname, QString& role)
{
    QSqlQuery query;
    query.prepare("SELECT username, email, phone, nickname, role FROM users WHERE user_id = ?");
    query.addBindValue(user_id);
    if (!query.exec() || !query.next()) {
        return false;
    }
    username = query.value(0).toString();
    email = query.value(1).toString();
    phone = query.value(2).toString();
    nickname = query.value(3).toString();
    role = query.value(4).toString();
    return true;
}

bool DatabaseManager::getUserIdByUsername(const QString& username, int& user_id)
{
    QSqlQuery query;
    query.prepare("SELECT user_id FROM users WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec() || !query.next()) {
        return false;
    }
    user_id = query.value(0).toInt();
    return true;
}

// 设备管理
bool DatabaseManager::addDevice(const QString& name, const QString& type, const QString& location,
                  const QString& manufacturer, const QString& model, const QString& installation_date)
{
    QSqlQuery query;
    query.prepare("INSERT INTO devices (name, type, location, manufacturer, model, installation_date) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(type);
    query.addBindValue(location);
    query.addBindValue(manufacturer);
    query.addBindValue(model);
    query.addBindValue(installation_date);
    return query.exec();
}

bool DatabaseManager::updateDevice(int device_id, const QString& name, const QString& type, const QString& location,
                     const QString& manufacturer, const QString& model, const QString& installation_date)
{
    QSqlQuery query;
    query.prepare("UPDATE devices SET name=?, type=?, location=?, manufacturer=?, model=?, installation_date=? WHERE device_id=?");
    query.addBindValue(name);
    query.addBindValue(type);
    query.addBindValue(location);
    query.addBindValue(manufacturer);
    query.addBindValue(model);
    query.addBindValue(installation_date);
    query.addBindValue(device_id);
    return query.exec();
}

bool DatabaseManager::deleteDevice(int device_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM devices WHERE device_id=?");
    query.addBindValue(device_id);
    return query.exec();
}

QVariantList DatabaseManager::getDevices()
{
    QVariantList devices;
    QSqlQuery query("SELECT device_id, name, type, location, manufacturer, model, installation_date FROM devices");
    while (query.next()) {
        QVariantMap device;
        device["device_id"] = query.value(0).toInt();
        device["name"] = query.value(1).toString();
        device["type"] = query.value(2).toString();
        device["location"] = query.value(3).toString();
        device["manufacturer"] = query.value(4).toString();
        device["model"] = query.value(5).toString();
        device["installation_date"] = query.value(6).toString();
        devices.append(device);
    }
    return devices;
}

bool DatabaseManager::getDeviceIdByName(const QString& name, int& device_id)
{
    QSqlQuery query;
    query.prepare("SELECT device_id FROM devices WHERE name = ?");
    query.addBindValue(name);
    if (!query.exec() || !query.next()) {
        return false;
    }
    device_id = query.value(0).toInt();
    return true;
}

bool DatabaseManager::getDeviceById(int device_id, QVariantMap& device)
{
    QSqlQuery query;
    query.prepare("SELECT device_id, name, type, location, manufacturer, model, installation_date FROM devices WHERE device_id=?");
    query.addBindValue(device_id);
    if (!query.exec() || !query.next()) {
        return false;
    }
    device["device_id"] = query.value(0).toInt();
    device["name"] = query.value(1).toString();
    device["type"] = query.value(2).toString();
    device["location"] = query.value(3).toString();
    device["manufacturer"] = query.value(4).toString();
    device["model"] = query.value(5).toString();
    device["installation_date"] = query.value(6).toString();
    return true;
}

// 监控数据
bool DatabaseManager::addMonitorData(int device_id, const QDateTime& timestamp,
                       double temperature, double humidity, double light)
{
    QSqlQuery query;
    query.prepare("INSERT INTO monitor_data (device_id, timestamp, temperature, humidity, light) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(device_id);
    query.addBindValue(timestamp);
    query.addBindValue(temperature);
    query.addBindValue(humidity);
    query.addBindValue(light);
    return query.exec();
}

QVariantList DatabaseManager::getDeviceData(int device_id, const QDateTime& startTime, const QDateTime& endTime)
{
    QVariantList dataList;
    QSqlQuery query;
    query.prepare("SELECT timestamp, temperature, humidity, light FROM monitor_data "
                  "WHERE device_id=? AND timestamp BETWEEN ? AND ? ORDER BY timestamp DESC");
    query.addBindValue(device_id);
    query.addBindValue(startTime);
    query.addBindValue(endTime);
    while (query.next()) {
        QVariantMap data;
        data["timestamp"] = query.value(0).toString();
        data["temperature"] = query.value(1).toDouble();
        data["humidity"] = query.value(2).toDouble();
        data["light"] = query.value(3).toDouble();
        dataList.append(data);
    }
    return dataList;
}

// 告警规则
bool DatabaseManager::addAlarmRule(int device_id, const QString& description, const QString& condition, const QString& action)
{
    QSqlQuery query;
    query.prepare("INSERT INTO alarm_rules (device_id, description, condition, action) VALUES (?, ?, ?, ?)");
    query.addBindValue(device_id);
    query.addBindValue(description);
    query.addBindValue(condition);
    query.addBindValue(action);
    return query.exec();
}

bool DatabaseManager::updateAlarmRule(int rule_id, const QString& description, const QString& condition, const QString& action)
{
    QSqlQuery query;
    query.prepare("UPDATE alarm_rules SET description=?, condition=?, action=? WHERE rule_id=?");
    query.addBindValue(description);
    query.addBindValue(condition);
    query.addBindValue(action);
    query.addBindValue(rule_id);
    return query.exec();
}

bool DatabaseManager::deleteAlarmRule(int rule_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM alarm_rules WHERE rule_id=?");
    query.addBindValue(rule_id);
    return query.exec();
}

QVariantList DatabaseManager::getAlarmRules(int device_id)
{
    QVariantList rules;
    QSqlQuery query;
    query.prepare("SELECT rule_id, description, condition, action FROM alarm_rules WHERE device_id=?");
    query.addBindValue(device_id);
    if (query.exec()) {
        while (query.next()) {
            QVariantMap rule;
            rule["rule_id"] = query.value(0).toInt();
            rule["description"] = query.value(1).toString();
            rule["condition"] = query.value(2).toString();
            rule["action"] = query.value(3).toString();
            rules.append(rule);
        }
    }
    return rules;
}

// 告警记录
bool DatabaseManager::addAlarmRecord(int device_id, const QDateTime& timestamp, const QString& content, const QString& status, const QString& note)
{
    QSqlQuery query;
    query.prepare("INSERT INTO alarm_records (device_id, timestamp, content, status, note) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(device_id);
    query.addBindValue(timestamp);
    query.addBindValue(content);
    query.addBindValue(status);
    query.addBindValue(note);
    return query.exec();
}

QVariantList DatabaseManager::getAlarmRecords(int device_id)
{
    QVariantList records;
    QSqlQuery query;
    query.prepare("SELECT alarm_id, timestamp, content, status, note FROM alarm_records WHERE device_id=?");
    query.addBindValue(device_id);
    if (query.exec()) {
        while (query.next()) {
            QVariantMap record;
            record["alarm_id"] = query.value(0).toInt();
            record["timestamp"] = query.value(1).toString();
            record["content"] = query.value(2).toString();
            record["status"] = query.value(3).toString();
            record["note"] = query.value(4).toString();
            records.append(record);
        }
    }
    return records;
}

// 系统日志
bool DatabaseManager::addLog(const QString& log_type, const QString& log_level, const QString& content,
                int user_id, int device_id)
{
    QSqlQuery query;
    query.prepare("INSERT INTO system_logs (timestamp, log_type, log_level, content, user_id, device_id) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(log_type);
    query.addBindValue(log_level);
    query.addBindValue(content);
    if (user_id == -1) query.addBindValue(QVariant(QVariant::Int)); else query.addBindValue(user_id);
    if (device_id == -1) query.addBindValue(QVariant(QVariant::Int)); else query.addBindValue(device_id);
    return query.exec();
}

QVariantList DatabaseManager::getLogs(const QDateTime& startTime, const QDateTime& endTime)
{
    QVariantList logs;
    QSqlQuery query;
    if (startTime.isValid() && endTime.isValid()) {
        query.prepare("SELECT log_id, timestamp, log_type, log_level, content, user_id, device_id FROM system_logs WHERE timestamp BETWEEN ? AND ? ORDER BY timestamp DESC");
        query.addBindValue(startTime);
        query.addBindValue(endTime);
    } else {
        query.prepare("SELECT log_id, timestamp, log_type, log_level, content, user_id, device_id FROM system_logs ORDER BY timestamp DESC");
    }
    if (query.exec()) {
        while (query.next()) {
            QVariantMap log;
            log["log_id"] = query.value(0).toInt();
            log["timestamp"] = query.value(1).toString();
            log["log_type"] = query.value(2).toString();
            log["log_level"] = query.value(3).toString();
            log["content"] = query.value(4).toString();
            log["user_id"] = query.value(5).toInt();
            log["device_id"] = query.value(6).toInt();
            logs.append(log);
        }
    }
    return logs;
}
