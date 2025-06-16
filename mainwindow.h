#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "logmanager.h"

// 前向声明
class AdminWindow;
class UserWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void Init(void);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLoginSuccess(bool isAdmin);
    void onLoginFailed(const QString& errorMsg);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& errorMsg);
    void onAdminWindowClosed();
    void onUserWindowClosed();

private:
    void showAdminWindow();
    void showUserWindow();
    void clearInputFields();

private:
    Ui::MainWindow *ui;
    LogManager *logManager;  // 登录管理器
    AdminWindow *adminWindow;  // 管理员窗口
    UserWindow *userWindow;    // 用户窗口
    QString *uesrName;
    QString *userPassword;
    bool role;
};
#endif // MAINWINDOW_H
