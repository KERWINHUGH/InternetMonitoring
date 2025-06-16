#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QWidget>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>


class LogManager : public QWidget
{
    Q_OBJECT
public:
    explicit LogManager(QWidget *parent = nullptr);

signals:
    void loginSuccess(bool isAdmin);  // 登录成功信号
    void loginFailed(const QString& errorMsg);  // 登录失败信号
    void registerSuccess();  // 注册成功信号
    void registerFailed(const QString& errorMsg);  // 注册失败信号

public:
    int loginAttempts;      //密码输入尝试次数
    
    // 公共方法
    void handleLogin(const QString& user, const QString& pwd);
    void handleRegister(const QString& user, const QString& pwd, bool isAdmin = false);
    void resetLoginAttempts();
    int getLoginAttempts() const;

private:
    bool checkInputValid(const QString& user, const QString& pwd, QString& errMsg);
    bool checkUser(const QString& user, const QString& pwd, bool& isAdmin, QString& errMsg);
    bool registerUser(const QString& user, const QString& pwd, bool isAdmin, QString& errMsg);

};

#endif // LOGMANAGER_H
