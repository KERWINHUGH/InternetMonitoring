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
    static DatabaseManager& instance()
    {
        static DatabaseManager instance;
        return instance;
    }
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
                const QString& email, const QString& phone,
                const QString& nickname, const QString& role);
    bool updateUser(int user_id, const QString& email,
                   const QString& phone, const QString& nickname);
    bool updatePassword(int user_id, const QString& newPassword);
    bool deleteUser(int user_id);
    bool verifyUser(const QString& username, const QString& password, int& user_id, QString& role);
    bool getUserInfo(int user_id, QString& username, QString& email,
                    QString& phone, QString& nickname, QString& role);
    bool getUserIdByUsername(const QString& username, int& user_id);

    // 设备管理
    bool addDevice(const QString& name, const QString& type, const QString& location,
                  const QString& manufacturer, const QString& model, const QString& installation_date);
    bool updateDevice(int device_id, const QString& name, const QString& type, const QString& location,
                     const QString& manufacturer, const QString& model, const QString& installation_date);
    bool deleteDevice(int device_id);
    QVariantList getDevices();
    bool getDeviceIdByName(const QString& name, int& device_id);
    bool getDeviceById(int device_id, QVariantMap& device);

    // 监控数据
    bool addMonitorData(int device_id, const QDateTime& timestamp,
                       double temperature, double humidity, double light);
    QVariantList getDeviceData(int device_id, const QDateTime& startTime, const QDateTime& endTime);

    // 告警规则
    bool addAlarmRule(int device_id, const QString& description, const QString& condition, const QString& action);
    bool updateAlarmRule(int rule_id, int device_id, const QString& description, const QString& condition, const QString& action);
    bool deleteAlarmRule(int rule_id);
    QVariantList getAlarmRules(int device_id);

    // 告警记录
    bool addAlarmRecord(int device_id, const QDateTime& timestamp, const QString& content, const QString& status, const QString& note);
    QVariantList getAlarmRecords(int device_id);
    QVariantList getAlarmRecordsFiltered(int device_id, const QString& status, const QDateTime& startTime, const QDateTime& endTime);

    // 系统日志
    bool addLog(const QString& log_type, const QString& log_level, const QString& content,
                int user_id = -1, int device_id = -1);
    QVariantList getLogs(const QDateTime& startTime = QDateTime(), const QDateTime& endTime = QDateTime());

    // 设备分组管理
    QVariantList getDeviceGroups(const QString& groupType); // groupType: "类型"/"位置"/"自定义"
    bool addDeviceGroup(const QString& groupName, const QString& groupType);
    bool renameDeviceGroup(int groupId, const QString& newName);
    bool deleteDeviceGroup(int groupId);
    bool setDeviceGroup(int deviceId, int groupId);
    QVariantList getDevicesByGroup(int groupId, bool isNullGroup = false);
    QVariantList getAllDeviceGroups();

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
    bool dropTables();
    bool executeQuery(const QString& sql);
    void setLastError(const QString& error);

    QSqlDatabase db;
    bool connected;
    QString lastErrorMsg;
};

#endif // DATABASEMANAGER_H 
