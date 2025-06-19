#include "loginmanager.h"
#include "databasemanager.h"
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>
#include <QRegularExpression>
#include <QTimer>
#include <QDateTime>

LoginManager::LoginManager(QObject *parent)
    : QObject(parent), loginAttempts(0), sessionTimer(new QTimer(this)),
      sessionTimeoutMinutes(DEFAULT_SESSION_TIMEOUT), sessionWarningEmitted(false)
{
    connect(sessionTimer, &QTimer::timeout, this, &LoginManager::checkSessionTimeout);
    sessionTimer->setInterval(60000); // 每分钟检查一次
    sessionTimer->start();
}

LoginManager::~LoginManager()
{
    sessionTimer->stop();
    delete sessionTimer;
}

void LoginManager::resetSessionTimer()
{
    lastActivityTime = QDateTime::currentDateTime();
    sessionWarningEmitted = false;
}

void LoginManager::setSessionTimeout(int minutes)
{
    if (minutes > 0) {
        sessionTimeoutMinutes = minutes;
        resetSessionTimer();
    }
}

void LoginManager::checkSessionTimeout()
{
    if (!isLoggedIn()) return;

    QDateTime currentTime = QDateTime::currentDateTime();
    int remainingMinutes = lastActivityTime.secsTo(currentTime) / 60;
    int remainingSeconds = sessionTimeoutMinutes - remainingMinutes;

    if (remainingMinutes >= sessionTimeoutMinutes) {
        // 会话超时，自动登出
        logout();
        emit sessionTimeout();
    } else if (remainingSeconds <= SESSION_WARNING_THRESHOLD * 60 && !sessionWarningEmitted) {
        // 发出警告
        emit sessionWarning(remainingSeconds);
        sessionWarningEmitted = true;
    }
}

void LoginManager::updateLastActivityTime()
{
    if (isLoggedIn()) {
        resetSessionTimer();
    }
}

bool LoginManager::isUsernameValid(const QString& username, QString& errorMsg)
{
    if (username.length() < MIN_USERNAME_LENGTH) {
        errorMsg = QString("用户名长度不能小于%1个字符").arg(MIN_USERNAME_LENGTH);
        return false;
    }
    if (username.length() > MAX_USERNAME_LENGTH) {
        errorMsg = QString("用户名长度不能超过%1个字符").arg(MAX_USERNAME_LENGTH);
        return false;
    }
    QRegularExpression regex("^[a-zA-Z0-9_]+$");
    if (!regex.match(username).hasMatch()) {
        errorMsg = "用户名只能包含字母、数字和下划线";
        return false;
    }
    return true;
}

bool LoginManager::isEmailValid(const QString& email, QString& errorMsg)
{
    if (email.isEmpty()) return true; // 允许为空

    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!regex.match(email).hasMatch()) {
        errorMsg = "邮箱格式不正确";
        return false;
    }
    return true;
}

bool LoginManager::isPhoneValid(const QString& phone, QString& errorMsg)
{
    if (phone.isEmpty()) return true; // 允许为空

    QRegularExpression regex("^1[3-9]\\d{9}$");
    if (!regex.match(phone).hasMatch()) {
        errorMsg = "手机号格式不正确";
        return false;
    }
    return true;
}

bool LoginManager::isNicknameValid(const QString& nickname, QString& errorMsg)
{
    if (nickname.isEmpty()) return true; // 允许为空

    if (nickname.length() < MIN_NICKNAME_LENGTH) {
        errorMsg = QString("昵称长度不能小于%1个字符").arg(MIN_NICKNAME_LENGTH);
        return false;
    }
    if (nickname.length() > MAX_NICKNAME_LENGTH) {
        errorMsg = QString("昵称长度不能超过%1个字符").arg(MAX_NICKNAME_LENGTH);
        return false;
    }
    return true;
}

bool LoginManager::isPasswordValid(const QString& password, QString& errorMsg)
{
    if (password.length() < MIN_PASSWORD_LENGTH) {
        errorMsg = QString("密码长度不能小于%1个字符").arg(MIN_PASSWORD_LENGTH);
        return false;
    }
    if (password.length() > MAX_PASSWORD_LENGTH) {
        errorMsg = QString("密码长度不能超过%1个字符").arg(MAX_PASSWORD_LENGTH);
        return false;
    }
    if (!isPasswordStrong(password)) {
        errorMsg = "密码必须包含大小写字母、数字和特殊字符中的至少三种";
        return false;
    }
    return true;
}

bool LoginManager::isPasswordStrong(const QString& password)
{
    int types = 0;
    if (password.contains(QRegularExpression("[A-Z]"))) types++;
    if (password.contains(QRegularExpression("[a-z]"))) types++;
    if (password.contains(QRegularExpression("[0-9]"))) types++;
    if (password.contains(QRegularExpression("[!@#$%^&*(),.?\":{}|<>]"))) types++;
    return types >= 3;
}

void LoginManager::handleLogin(const QString& username, const QString& password)
{
    // 输入验证
    if (username.isEmpty() || password.isEmpty()) {
        emit loginFailed("用户名和密码不能为空");
        return;
    }

    // 尝试登录
    int user_id;
    QString role;
    if (DatabaseManager::instance().verifyUser(username, password, user_id, role)) {
        currentUsername = username;
        currentUserRole = role;
        resetSessionTimer();
        DatabaseManager::instance().addLog("登录", "INFO", QString("用户 %1 登录成功").arg(username), user_id);
        emit loginSuccess(role == "admin");
        loginAttempts = 0;
    } else {
        DatabaseManager::instance().addLog("登录", "WARN", QString("用户 %1 登录失败").arg(username));
        loginAttempts++;
        emit loginFailed("用户名或密码错误");
    }
}

void LoginManager::handleRegister(const QString& username, const QString& password, bool isAdmin)
{
    handleRegister(username, password, QString(), QString(), isAdmin);
}

void LoginManager::handleRegister(const QString& username, const QString& password, const QString& email, const QString& phone, bool isAdmin)
{
    // 输入验证
    QString errorMsg;
    if (!isUsernameValid(username, errorMsg)) {
        emit registerFailed(errorMsg);
        return;
    }
    if (!isPasswordValid(password, errorMsg)) {
        emit registerFailed(errorMsg);
        return;
    }
    if (!isEmailValid(email, errorMsg)) {
        emit registerFailed(errorMsg);
        return;
    }
    if (!isPhoneValid(phone, errorMsg)) {
        emit registerFailed(errorMsg);
        return;
    }
    // 尝试注册
    if (DatabaseManager::instance().addUser(username, password, email, phone, username, isAdmin ? "admin" : "user")) {
        int user_id;
        DatabaseManager::instance().getUserIdByUsername(username, user_id);
        DatabaseManager::instance().addLog("注册", "INFO", QString("用户 %1 注册成功").arg(username), user_id);
        emit registerSuccess();
    } else {
        DatabaseManager::instance().addLog("注册", "WARN", QString("用户 %1 注册失败，用户名可能已存在").arg(username));
        emit registerFailed("注册失败，用户名可能已存在");
    }
}

bool LoginManager::resetPassword(const QString& username, const QString& oldPassword,
                               const QString& newPassword)
{
    // 验证新密码强度
    QString errorMsg;
    if (!isPasswordValid(newPassword, errorMsg)) {
        return false;
    }

    int user_id;
    QString role;
    if (!DatabaseManager::instance().verifyUser(username, oldPassword, user_id, role)) {
        return false;
    }
    if (DatabaseManager::instance().updatePassword(user_id, newPassword)) {
        DatabaseManager::instance().addLog("密码修改", "INFO", QString("用户 %1 修改了密码").arg(username), user_id);
        return true;
    }
    return false;
}

bool LoginManager::updateUserInfo(const QString& username, const QString& email,
                                const QString& phone, const QString& nickname)
{
    // 输入验证
    QString errorMsg;
    if (!isEmailValid(email, errorMsg)) return false;
    if (!isPhoneValid(phone, errorMsg)) return false;
    if (!isNicknameValid(nickname, errorMsg)) return false;

    int user_id;
    if (!DatabaseManager::instance().getUserIdByUsername(username, user_id)) {
        return false;
    }
    if (DatabaseManager::instance().updateUser(user_id, email, phone, nickname)) {
        DatabaseManager::instance().addLog("用户信息修改", "INFO", QString("用户 %1 修改了个人信息").arg(username), user_id);
        return true;
    }
    return false;
}

bool LoginManager::getUserInfo(const QString& username, QString& email,
                             QString& phone, QString& nickname, QString& role)
{
    int user_id;
    if (!DatabaseManager::instance().getUserIdByUsername(username, user_id)) {
        return false;
    }
    QString fetchedUsername;
    return DatabaseManager::instance().getUserInfo(user_id, fetchedUsername, email, phone, nickname, role);
}

void LoginManager::logout()
{
    if (!currentUsername.isEmpty()) {
        int user_id;
        if (DatabaseManager::instance().getUserIdByUsername(currentUsername, user_id)) {
            DatabaseManager::instance().addLog("登出", "INFO", QString("用户 %1 登出系统").arg(currentUsername), user_id);
        }
    }
    currentUsername.clear();
    currentUserRole.clear();
    sessionTimer->stop();
}
