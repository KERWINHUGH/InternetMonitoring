#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QVariantMap>
#include <QDebug>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();
    bool initDatabase();

    // 数据库状态
    bool isConnected() const { return connected; }
    QString lastError() const { return lastErrorMsg; }
    void clearError() { lastErrorMsg.clear(); }

    // 事务控制
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // 用户管理
    bool addUser(const QString& username, const QString& password,
                const QString& email = QString(),
                const QString& phone = QString(),
                const QString& nickname = QString(),
                const QString& role = "user");
    bool updateUser(const QString& username, const QString& email,
                   const QString& phone, const QString& nickname);
    bool updatePassword(const QString& username, const QString& newPassword);
    bool deleteUser(const QString& username);
    bool verifyUser(const QString& username, const QString& password, QString& role);
    bool getUserInfo(const QString& username, QString& email,
                    QString& phone, QString& nickname, QString& role);

    // 设备管理
    bool addDevice(const QString& deviceId, const QString& name,
                  const QString& type, const QString& location);
    bool updateDevice(const QString& deviceId, const QString& name,
                     const QString& type, const QString& location);
    bool deleteDevice(const QString& deviceId);
    QVariantList getDevices();

    // 监控数据
    bool addMonitorData(const QString& deviceId, double temperature,
                       double humidity, double cpuUsage,
                       double memoryUsage, double networkSpeed);
    QVariantList getDeviceData(const QString& deviceId,
                              const QDateTime& startTime,
                              const QDateTime& endTime);

    // 告警管理
    bool addAlarm(const QString& deviceId, const QString& type,
                 const QString& description, const QString& level);
    bool updateAlarmStatus(int alarmId, const QString& status);
    QVariantList getAlarms(const QString& deviceId = QString(),
                          const QDateTime& startTime = QDateTime(),
                          const QDateTime& endTime = QDateTime());

    // 系统日志
    bool addLog(const QString& type, const QString& content,
                const QString& username);
    QVariantList getLogs(const QDateTime& startTime = QDateTime(),
                        const QDateTime& endTime = QDateTime());

signals:
    void databaseError(const QString& error);
    void databaseConnected();
    void databaseDisconnected();

private:
    DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    bool executeQuery(const QString& sql);
    void setLastError(const QString& error);

    QSqlDatabase db;
    bool connected;
    QString lastErrorMsg;
};

#endif // DATABASEMANAGER_H 