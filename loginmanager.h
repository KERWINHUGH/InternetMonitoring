#ifndef LOGINMANAGER_H
#define LOGINMANAGER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

class LoginManager : public QObject
{
    Q_OBJECT

public:
    explicit LoginManager(QObject *parent = nullptr);
    ~LoginManager();

    // 用户登录相关
    void handleLogin(const QString& username, const QString& password);
    void handleRegister(const QString& username, const QString& password, bool isAdmin);
    bool resetPassword(const QString& username, const QString& oldPassword,
                      const QString& newPassword);

    // 用户信息管理
    bool updateUserInfo(const QString& username, const QString& email,
                       const QString& phone, const QString& nickname);
    bool getUserInfo(const QString& username, QString& email,
                    QString& phone, QString& nickname, QString& role);

    // 登录状态管理
    int getLoginAttempts() const { return loginAttempts; }
    void resetLoginAttempts() { loginAttempts = 0; }
    void logout();
    bool isLoggedIn() const { return !currentUsername.isEmpty(); }
    QString getCurrentUsername() const { return currentUsername; }
    QString getCurrentUserRole() const { return currentUserRole; }

    // 会话管理
    void resetSessionTimer();
    void setSessionTimeout(int minutes);
    int getSessionTimeout() const { return sessionTimeoutMinutes; }
    QDateTime getLastActivityTime() const { return lastActivityTime; }

    // 密码验证
    static bool isPasswordValid(const QString& password, QString& errorMsg);
    static bool isPasswordStrong(const QString& password);

    // 输入验证
    static bool isUsernameValid(const QString& username, QString& errorMsg);
    static bool isEmailValid(const QString& email, QString& errorMsg);
    static bool isPhoneValid(const QString& phone, QString& errorMsg);
    static bool isNicknameValid(const QString& nickname, QString& errorMsg);

signals:
    void loginSuccess(bool isAdmin);
    void loginFailed(const QString& errorMsg);
    void registerSuccess();
    void registerFailed(const QString& errorMsg);
    void sessionTimeout();
    void sessionWarning(int remainingSeconds);

private slots:
    void checkSessionTimeout();
    void updateLastActivityTime();

private:
    int loginAttempts;
    QString currentUsername;
    QString currentUserRole;

    // 会话管理
    QTimer *sessionTimer;
    QDateTime lastActivityTime;
    int sessionTimeoutMinutes;
    bool sessionWarningEmitted;

    // 密码验证规则
    static const int MIN_PASSWORD_LENGTH = 6;
    static const int MAX_PASSWORD_LENGTH = 20;

    // 用户名验证规则
    static const int MIN_USERNAME_LENGTH = 3;
    static const int MAX_USERNAME_LENGTH = 20;

    // 昵称验证规则
    static const int MIN_NICKNAME_LENGTH = 2;
    static const int MAX_NICKNAME_LENGTH = 20;

    // 会话设置
    static const int DEFAULT_SESSION_TIMEOUT = 30;  // 30分钟
    static const int SESSION_WARNING_THRESHOLD = 5; // 剩余5分钟时发出警告
};

#endif // LOGINMANAGER_H
