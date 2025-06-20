#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPoint>
#include "loginmanager.h"

// 前向声明
class AdminWindow;
class UserWindow;
class RegisterWindow;
class ForgetPasswordWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLoginSuccess(bool isAdmin);
    void onLoginFailed(const QString& errorMsg);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& errorMsg);
    void onAdminWindowClosed();
    void onUserWindowClosed();
    void onShowPasswordClicked();
    void onMinClicked();
    void onMaxClicked();
    void onCloseClicked();
    void onSessionTimeout();
    void onSessionWarning(int remainingSeconds);
    void onRegisterWindowBack();
    void onForgetPasswordWindowBack();
    void onForgetPasswordClicked();

private:
    Ui::MainWindow *ui;
    LoginManager *logManager;  // 登录管理器
    AdminWindow *adminWindow;  // 管理员窗口
    UserWindow *userWindow;    // 用户窗口
    bool isPasswordVisible;
    bool isMaximized;
    QPoint dragPosition;
    RegisterWindow *registerWindow;
    ForgetPasswordWindow *forgetPasswordWindow;
    QString currentUsername;   // 当前登录用户名

    void initUI();
    void setupConnections();
    void clearInputFields();
    void showError(const QString& message);
    void showLoginWindow();
};

#endif // MAINWINDOW_H
