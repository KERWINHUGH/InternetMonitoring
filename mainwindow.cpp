#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QApplication>
#include <QIcon>
#include "loginmanager.h"
#include "adminwindow.h"
#include "userwindow.h"
#include "registerwindow.h"
#include "forgetpasswordwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logManager(new LoginManager(this))
    , adminWindow(nullptr)
    , userWindow(nullptr)
    , registerWindow(nullptr)
    , forgetPasswordWindow(nullptr)
    , isPasswordVisible(false)
    , isMaximized(false)
{
    ui->setupUi(this);
    initUI();
    setupConnections();
}

/**
 * @brief 初始化主窗口界面
 *
 * 该函数负责初始化主窗口的各种控件和信号槽连接：
 * 1. 连接窗口控制按钮（关闭、最小化、最大化）
 * 2. 设置输入框的占位符文本
 * 3. 连接登录和注册按钮的信号槽
 * 4. 连接 LoginManager 的信号槽
 */
void MainWindow::initUI()
{
    // 设置窗口属性 - 移除透明背景，保持无边框
    setWindowFlags(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground); // 注释掉透明背景

    // 设置窗口标题和图标
    setWindowTitle("物联网设备监控系统");
    setWindowIcon(QIcon(":/img/IOT.png"));

    // 设置窗口大小
    resize(400, 500);
    setFixedSize(400, 500);

    // 设置样式表
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f0f0;
            border: 2px solid #cccccc;
            border-radius: 10px;
        }
        
        QLabel#titleLabel {
            color: #333333;
            font-size: 16px;
            font-weight: bold;
            padding: 10px;
        }
        
        QLineEdit {
            border: 2px solid #cccccc;
            border-radius: 5px;
            padding: 8px;
            font-size: 14px;
            background-color: white;
        }
        
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 10px;
            font-size: 14px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #45a049;
        }
        
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        
        QPushButton#btnMin, QPushButton#btnMax, QPushButton#btnClose {
            background-color: transparent;
            color: #666666;
            border: none;
            border-radius: 3px;
            min-width: 30px;
            max-width: 30px;
            min-height: 30px;
            max-height: 30px;
        }
        
        QPushButton#btnMin:hover, QPushButton#btnMax:hover {
            background-color: #e0e0e0;
        }
        
        QPushButton#btnClose:hover {
            background-color: #ff4444;
            color: white;
        }
        
        QPushButton#btnShowPassword {
            background-color: #2196F3;
            min-width: 60px;
            max-width: 60px;
        }
        
        QPushButton#btnShowPassword:hover {
            background-color: #1976D2;
        }
        
        QLabel {
            color: #333333;
            font-size: 14px;
        }
    )");

    // 设置密码框属性
    ui->editPassword->setEchoMode(QLineEdit::Password);

    // 设置输入框占位符文本
    ui->editUserAccount->setPlaceholderText("请输入用户名");
    ui->editPassword->setPlaceholderText("请输入密码");

    // 设置输入验证器
    QRegularExpression userRegExp("[a-zA-Z0-9_]{3,20}");
    ui->editUserAccount->setValidator(new QRegularExpressionValidator(userRegExp, this));

    QRegularExpression pwdRegExp(".{6,20}");
    ui->editPassword->setValidator(new QRegularExpressionValidator(pwdRegExp, this));
}

void MainWindow::setupConnections()
{
    // 连接登录和注册按钮
    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(ui->btnForgetPassword, &QPushButton::clicked, this, &MainWindow::onForgetPasswordClicked);

    // 连接窗口控制按钮
    connect(ui->btnMin, &QPushButton::clicked, this, &MainWindow::onMinClicked);
    connect(ui->btnMax, &QPushButton::clicked, this, &MainWindow::onMaxClicked);
    connect(ui->btnClose, &QPushButton::clicked, this, &MainWindow::onCloseClicked);
    connect(ui->btnShowPassword, &QPushButton::clicked, this, &MainWindow::onShowPasswordClicked);

    // 连接回车键
    connect(ui->editUserAccount, &QLineEdit::returnPressed, this, &MainWindow::onLoginClicked);
    connect(ui->editPassword, &QLineEdit::returnPressed, this, &MainWindow::onLoginClicked);

    // 连接 LoginManager 信号槽
    connect(logManager, &LoginManager::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(logManager, &LoginManager::loginFailed, this, &MainWindow::onLoginFailed);
    connect(logManager, &LoginManager::registerSuccess, this, &MainWindow::onRegisterSuccess);
    connect(logManager, &LoginManager::registerFailed, this, &MainWindow::onRegisterFailed);
    connect(logManager, &LoginManager::sessionTimeout, this, &MainWindow::onSessionTimeout);
    connect(logManager, &LoginManager::sessionWarning, this, &MainWindow::onSessionWarning);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

/**
 * @brief 登录按钮点击事件处理函数
 *
 * 该函数负责处理用户登录逻辑：
 * 1. 获取用户输入的用户名和密码
 * 2. 委托给 LoginManager 处理登录逻辑
 */
void MainWindow::onLoginClicked()
{
    QString username = ui->editUserAccount->text().trimmed();
    QString password = ui->editPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "错误", "用户名和密码不能为空");
        return;
    }

    logManager->handleLogin(username, password);
}

/**
 * @brief 注册按钮点击事件处理函数
 *
 * 该函数负责处理用户注册逻辑：
 * 1. 获取用户输入的用户名和密码
 * 2. 委托给 LoginManager 处理注册逻辑
 */
void MainWindow::onRegisterClicked()
{
    if (!registerWindow) {
        registerWindow = new RegisterWindow(this);
        connect(registerWindow, &RegisterWindow::backToLogin, this, &MainWindow::onRegisterWindowBack);
    }
    this->hide();
    registerWindow->show();
}

/**
 * @brief 登录成功处理函数
 * @param isAdmin 是否为管理员
 */
void MainWindow::onLoginSuccess(bool isAdmin)
{
    QMessageBox::information(this, "登录成功", 
        isAdmin ? "管理员登录成功" : "用户登录成功");
    
    hide();
    if (isAdmin) {
        if (!adminWindow) {
            adminWindow = new AdminWindow();
            connect(adminWindow, &AdminWindow::windowClosed, 
                    this, &MainWindow::onAdminWindowClosed);
        }
        adminWindow->show();
    } else {
        if (!userWindow) {
            userWindow = new UserWindow();
            connect(userWindow, &UserWindow::windowClosed, 
                    this, &MainWindow::onUserWindowClosed);
        }
        userWindow->show();
    }
    clearInputFields();
}

/**
 * @brief 登录失败处理函数
 * @param errorMsg 错误信息
 */
void MainWindow::onLoginFailed(const QString& errorMsg)
{
    QMessageBox::warning(this, "登录失败", errorMsg);
    if (logManager->getLoginAttempts() >= 3) {
        QMessageBox::critical(this, "错误", "尝试次数过多，程序将关闭");
        QApplication::quit();
    }
}

/**
 * @brief 注册成功处理函数
 */
void MainWindow::onRegisterSuccess()
{
    QMessageBox::information(this, "注册成功", "请登录");
    clearInputFields();
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
 * @brief 管理员窗口关闭处理
 */
void MainWindow::onAdminWindowClosed()
{
    if (adminWindow) {
        adminWindow->hide();
    }
    show();
    clearInputFields();
    logManager->resetLoginAttempts();
}

/**
 * @brief 用户窗口关闭处理
 */
void MainWindow::onUserWindowClosed()
{
    if (userWindow) {
        userWindow->hide();
    }
    show();
    clearInputFields();
    logManager->resetLoginAttempts();
}

/**
 * @brief 清空输入框
 */
void MainWindow::clearInputFields()
{
    ui->editUserAccount->clear();
    ui->editPassword->clear();
    ui->editUserAccount->setFocus();
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

void MainWindow::onShowPasswordClicked()
{
    isPasswordVisible = !isPasswordVisible;
    ui->editPassword->setEchoMode(
        isPasswordVisible ? QLineEdit::Normal : QLineEdit::Password);
    ui->btnShowPassword->setText(isPasswordVisible ? "隐藏" : "显示");
}

void MainWindow::onMinClicked()
{
    showMinimized();
}

void MainWindow::onMaxClicked()
{
    if (isMaximized) {
        showNormal();
        ui->btnMax->setText("□");
    } else {
        showMaximized();
        ui->btnMax->setText("❐");
    }
    isMaximized = !isMaximized;
}

void MainWindow::onCloseClicked()
{
    close();
}

void MainWindow::onSessionTimeout()
{
    if (adminWindow && adminWindow->isVisible()) {
        adminWindow->close();
    }
    if (userWindow && userWindow->isVisible()) {
        userWindow->close();
    }
    show();
    QMessageBox::warning(this, "会话超时", "由于长时间未操作，您已被自动登出");
    clearInputFields();
}

void MainWindow::onSessionWarning(int remainingSeconds)
{
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    QString message = QString("您的会话将在 %1分%2秒 后超时，请及时保存工作。")
                         .arg(minutes)
                         .arg(seconds);
    
    QWidget* activeWindow = QApplication::activeWindow();
    if (activeWindow) {
        QMessageBox::warning(activeWindow, "会话即将超时", message);
    }
}

void MainWindow::onRegisterWindowBack()
{
    if (registerWindow) registerWindow->hide();
    this->show();
}

void MainWindow::onForgetPasswordClicked()
{
    if (!forgetPasswordWindow) {
        forgetPasswordWindow = new ForgetPasswordWindow(this);
        connect(forgetPasswordWindow, &ForgetPasswordWindow::backToLogin, this, &MainWindow::onForgetPasswordWindowBack);
    }
    this->hide();
    forgetPasswordWindow->show();
}

void MainWindow::onForgetPasswordWindowBack()
{
    if (forgetPasswordWindow) forgetPasswordWindow->hide();
    this->show();
}
