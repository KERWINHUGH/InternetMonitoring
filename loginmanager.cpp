#include "loginmanager.h"

LoginManager::LoginManager(QWidget *parent) : QWidget(parent)
{
    loginAttempts = 0;  // 初始化登录尝试次数
}

/**
 * @brief 验证用户输入的有效性
 * @param user 用户名
 * @param pwd 密码
 * @param errMsg 错误信息输出参数
 * @return 输入是否有效
 *
 * 该函数检查：
 * 1. 用户名和密码是否为空
 * 2. 是否包含非法字符（空格、分号等）
 */
bool LoginManager::checkInputValid(const QString& user, const QString& pwd, QString& errMsg)
{
    // 检查是否为空
    if (user.isEmpty() || pwd.isEmpty()) {
        errMsg = "用户名和密码不能为空";
        return false;
    }
    // 检查是否包含非法字符
    if (user.contains(" ") || pwd.contains(" ") || user.contains(";") || pwd.contains(";")) {
        errMsg = "用户名和密码不能包含空格或特殊字符";
        return false;
    }
    return true;
}

/**
 * @brief 检查用户凭据
 * @param user 用户名
 * @param pwd 密码
 * @param isAdmin 是否为管理员输出参数
 * @param errMsg 错误信息输出参数
 * @return 用户凭据是否正确
 *
 * 该函数从用户文件中读取用户信息并验证：
 * 1. 打开用户数据文件
 * 2. 逐行读取用户信息
 * 3. 验证用户名和密码是否匹配
 * 4. 确定用户角色
 */
bool LoginManager::checkUser(const QString& user, const QString& pwd, bool& isAdmin, QString& errMsg)
{
    QFile file("D:/code/QT/project/InternetMonitoring/users.txt");                       // 用户数据文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errMsg = "无法读取用户数据";
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();              // 读取一行
        QStringList parts = line.split(",");       // 按逗号分割
        // 检查格式：用户名,密码,角色
        if (parts.size() == 3 && parts[0] == user && parts[1] == pwd) {
            isAdmin = (parts[2] == "admin");       // 确定用户角色
            return true;
        }
    }
    errMsg = "用户名或密码错误";
    return false;
}

/**
 * @brief 注册新用户
 * @param user 用户名
 * @param pwd 密码
 * @param isAdmin 是否为管理员
 * @param errMsg 错误信息输出参数
 * @return 注册是否成功
 *
 * 该函数负责注册新用户：
 * 1. 检查用户名是否已存在
 * 2. 将新用户信息写入文件
 * 3. 文件格式：用户名,密码,角色
 */
bool LoginManager::registerUser(const QString& user, const QString& pwd, bool isAdmin, QString& errMsg)
{
    QFile file("D:/code/QT/project/InternetMonitoring/users.txt");                       // 用户数据文件
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        errMsg = "无法写入用户数据";
        return false;
    }

    // 检查用户名是否已存在
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();              // 读取一行
        QStringList parts = line.split(",");       // 按逗号分割
        if (parts.size() >= 1 && parts[0] == user) {
            errMsg = "用户名已存在";
            return false;
        }
    }

    // 写入新用户信息
    QTextStream out(&file);
    out << user << "," << pwd << "," << (isAdmin ? "admin" : "user") << "\n";
    return true;
}

/**
 * @brief 处理用户登录
 * @param user 用户名
 * @param pwd 密码
 * 
 * 该函数负责处理完整的登录逻辑：
 * 1. 验证输入有效性
 * 2. 检查用户凭据
 * 3. 处理登录尝试次数限制
 * 4. 发送相应的信号
 */
void LoginManager::handleLogin(const QString& user, const QString& pwd)
{
    QString errMsg;
    bool isAdmin = false;

    // 验证输入有效性
    if (!checkInputValid(user, pwd, errMsg)) {
        emit loginFailed(errMsg);
        return;
    }

    // 检查用户凭据
    if (checkUser(user, pwd, isAdmin, errMsg)) {
        loginAttempts = 0;  // 重置登录尝试次数
        emit loginSuccess(isAdmin);
    } else {
        loginAttempts++;  // 增加登录尝试次数
        emit loginFailed(errMsg);
    }
}

/**
 * @brief 处理用户注册
 * @param user 用户名
 * @param pwd 密码
 * @param isAdmin 是否为管理员
 * 
 * 该函数负责处理完整的注册逻辑：
 * 1. 验证输入有效性
 * 2. 注册新用户
 * 3. 发送相应的信号
 */
void LoginManager::handleRegister(const QString& user, const QString& pwd, bool isAdmin)
{
    QString errMsg;

    // 验证输入有效性
    if (!checkInputValid(user, pwd, errMsg)) {
        emit registerFailed(errMsg);
        return;
    }

    // 注册用户
    if (registerUser(user, pwd, isAdmin, errMsg)) {
        emit registerSuccess();
    } else {
        emit registerFailed(errMsg);
    }
}

/**
 * @brief 重置登录尝试次数
 */
void LoginManager::resetLoginAttempts()
{
    loginAttempts = 0;
}

/**
 * @brief 获取当前登录尝试次数
 * @return 登录尝试次数
 */
int LoginManager::getLoginAttempts() const
{
    return loginAttempts;
}
