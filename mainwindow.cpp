#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logmanager.h"
#include "adminwindow.h"
#include "userwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logManager(new LogManager(this))
    , adminWindow(nullptr)
    , userWindow(nullptr)
{
    ui->setupUi(this);
    Init();
}

/**
 * @brief 初始化主窗口界面
 * 
 * 该函数负责初始化主窗口的各种控件和信号槽连接：
 * 1. 连接窗口控制按钮（关闭、最小化、最大化）
 * 2. 设置输入框的占位符文本
 * 3. 连接登录和注册按钮的信号槽
 * 4. 连接 LogManager 的信号槽
 */
void MainWindow::Init()
{
    // 连接窗口控制按钮信号槽
    connect(ui->bntClose, &QPushButton::clicked, this, &QMainWindow::close);        // 关闭按钮
    connect(ui->bntMin, &QPushButton::clicked, this, &QMainWindow::showMinimized);  // 最小化按钮
    connect(ui->bntMax, &QPushButton::clicked, this, &QMainWindow::showMaximized);  // 最大化按钮

    // 设置输入框占位符文本
    ui->editUserAccount->setPlaceholderText("请输入用户名");   // 用户名输入框提示
    ui->editUserPassord->setPlaceholderText("请输入密码");     // 密码输入框提示

    // 连接功能按钮信号槽
    connect(ui->bntLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(ui->bntRegister, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    
    // 连接 LogManager 信号槽
    connect(logManager, &LogManager::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(logManager, &LogManager::loginFailed, this, &MainWindow::onLoginFailed);
    connect(logManager, &LogManager::registerSuccess, this, &MainWindow::onRegisterSuccess);
    connect(logManager, &LogManager::registerFailed, this, &MainWindow::onRegisterFailed);
}

/**
 * @brief 登录按钮点击事件处理函数
 * 
 * 该函数负责处理用户登录逻辑：
 * 1. 获取用户输入的用户名和密码
 * 2. 委托给 LogManager 处理登录逻辑
 */
void MainWindow::onLoginClicked()
{
    QString user = ui->editUserAccount->text();    // 获取用户名
    QString pwd = ui->editUserPassord->text();     // 获取密码
    
    // 委托给 LogManager 处理登录
    logManager->handleLogin(user, pwd);
}

/**
 * @brief 注册按钮点击事件处理函数
 * 
 * 该函数负责处理用户注册逻辑：
 * 1. 获取用户输入的用户名和密码
 * 2. 委托给 LogManager 处理注册逻辑
 */
void MainWindow::onRegisterClicked()
{
    QString user = ui->editUserAccount->text();    // 获取用户名
    QString pwd = ui->editUserPassord->text();     // 获取密码
    
    // 委托给 LogManager 处理注册（默认为普通用户）
    logManager->handleRegister(user, pwd, false);
}

/**
 * @brief 登录成功处理函数
 * @param isAdmin 是否为管理员
 */
void MainWindow::onLoginSuccess(bool isAdmin)
{
    role = isAdmin;  // 设置用户角色
    QMessageBox::information(this, "登录成功", isAdmin ? "管理员登录成功" : "用户登录成功");
    
    // 隐藏登录窗口
    this->hide();
    
    // 根据用户角色跳转到相应界面
    if (isAdmin) {
        showAdminWindow();
    } else {
        showUserWindow();
    }
}

/**
 * @brief 登录失败处理函数
 * @param errorMsg 错误信息
 */
void MainWindow::onLoginFailed(const QString& errorMsg)
{
    QMessageBox::warning(this, "登录失败", errorMsg);
    
    // 检查是否超过最大尝试次数
    if (logManager->getLoginAttempts() >= 3) {
        QMessageBox::critical(this, "错误", "尝试次数过多，程序将关闭");
        qApp->quit();
    }
}

/**
 * @brief 注册成功处理函数
 */
void MainWindow::onRegisterSuccess()
{
    QMessageBox::information(this, "注册成功", "请登录");
    clearInputFields();  // 清空输入框
}

/**
 * @brief 注册失败处理函数
 * @param errorMsg 错误信息
 */
void MainWindow::onRegisterFailed(const QString& errorMsg)
{
    QMessageBox::warning(this, "注册失败", errorMsg);
}

/**
 * @brief 显示管理员窗口
 */
void MainWindow::showAdminWindow()
{
    if (!adminWindow) {
        adminWindow = new AdminWindow();
        connect(adminWindow, &AdminWindow::windowClosed, this, &MainWindow::onAdminWindowClosed);
    }
    adminWindow->show();
}

/**
 * @brief 显示用户窗口
 */
void MainWindow::showUserWindow()
{
    if (!userWindow) {
        userWindow = new UserWindow();
        connect(userWindow, &UserWindow::windowClosed, this, &MainWindow::onUserWindowClosed);
    }
    userWindow->show();
}

/**
 * @brief 管理员窗口关闭处理
 */
void MainWindow::onAdminWindowClosed()
{
    if (adminWindow) {
        adminWindow->hide();
    }
    this->show();  // 显示登录窗口
    clearInputFields();  // 清空输入框
    logManager->resetLoginAttempts();  // 重置登录尝试次数
}

/**
 * @brief 用户窗口关闭处理
 */
void MainWindow::onUserWindowClosed()
{
    if (userWindow) {
        userWindow->hide();
    }
    this->show();  // 显示登录窗口
    clearInputFields();  // 清空输入框
    logManager->resetLoginAttempts();  // 重置登录尝试次数
}

/**
 * @brief 清空输入框
 */
void MainWindow::clearInputFields()
{
    ui->editUserAccount->clear();
    ui->editUserPassord->clear();
}

/**
 * @brief 主窗口析构函数
 * 
 * 该函数负责清理资源：
 * 1. 释放UI对象内存
 * 2. 释放子窗口内存
 */
MainWindow::~MainWindow()
{
    delete ui;
    if (adminWindow) {
        delete adminWindow;
    }
    if (userWindow) {
        delete userWindow;
    }
}
