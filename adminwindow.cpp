#include "adminwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createMenuBar();
    
    // 设置窗口属性
    setWindowTitle("管理员控制台");
    setFixedSize(800, 600);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
}

void AdminWindow::setupUI()
{
    // 创建中央部件
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    
    // 欢迎标签
    welcomeLabel = new QLabel("欢迎使用管理员控制台", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #2c3e50; margin: 20px; }");
    mainLayout->addWidget(welcomeLabel);
    
    // 功能按钮
    userManagementBtn = new QPushButton("用户管理", this);
    userManagementBtn->setFixedSize(200, 50);
    userManagementBtn->setStyleSheet(
        "QPushButton { "
        "    background-color: #3498db; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #2980b9; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #21618c; "
        "}"
    );
    
    systemSettingsBtn = new QPushButton("系统设置", this);
    systemSettingsBtn->setFixedSize(200, 50);
    systemSettingsBtn->setStyleSheet(
        "QPushButton { "
        "    background-color: #e74c3c; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #c0392b; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #a93226; "
        "}"
    );
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(userManagementBtn);
    buttonLayout->addWidget(systemSettingsBtn);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 底部按钮
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    logoutBtn = new QPushButton("退出登录", this);
    logoutBtn->setFixedSize(120, 40);
    logoutBtn->setStyleSheet(
        "QPushButton { "
        "    background-color: #95a5a6; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 6px; "
        "    font-size: 14px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #7f8c8d; "
        "}"
    );
    
    closeBtn = new QPushButton("关闭程序", this);
    closeBtn->setFixedSize(120, 40);
    closeBtn->setStyleSheet(
        "QPushButton { "
        "    background-color: #34495e; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 6px; "
        "    font-size: 14px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #2c3e50; "
        "}"
    );
    
    bottomLayout->addStretch();
    bottomLayout->addWidget(logoutBtn);
    bottomLayout->addWidget(closeBtn);
    mainLayout->addLayout(bottomLayout);
    
    // 连接信号槽
    connect(userManagementBtn, &QPushButton::clicked, this, &AdminWindow::onUserManagementClicked);
    connect(systemSettingsBtn, &QPushButton::clicked, this, &AdminWindow::onSystemSettingsClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &AdminWindow::onLogoutClicked);
    connect(closeBtn, &QPushButton::clicked, this, &AdminWindow::onCloseClicked);
    
    // 添加状态栏
    statusBar()->showMessage("管理员控制台已就绪");
}

void AdminWindow::createMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件");
    QAction *logoutAction = fileMenu->addAction("退出登录");
    QAction *exitAction = fileMenu->addAction("退出程序");
    
    // 管理菜单
    QMenu *manageMenu = menuBar->addMenu("管理");
    QAction *userAction = manageMenu->addAction("用户管理");
    QAction *settingsAction = manageMenu->addAction("系统设置");
    
    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助");
    QAction *aboutAction = helpMenu->addAction("关于");
    
    // 连接菜单信号
    connect(logoutAction, &QAction::triggered, this, &AdminWindow::onLogoutClicked);
    connect(exitAction, &QAction::triggered, this, &AdminWindow::onCloseClicked);
    connect(userAction, &QAction::triggered, this, &AdminWindow::onUserManagementClicked);
    connect(settingsAction, &QAction::triggered, this, &AdminWindow::onSystemSettingsClicked);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "关于", "管理员控制台 v1.0\n\n网络监控系统管理员界面");
    });
}

void AdminWindow::onUserManagementClicked()
{
    QMessageBox::information(this, "用户管理", "用户管理功能正在开发中...");
}

void AdminWindow::onSystemSettingsClicked()
{
    QMessageBox::information(this, "系统设置", "系统设置功能正在开发中...");
}

void AdminWindow::onLogoutClicked()
{
    int result = QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        this->close();
        // 发送信号通知主窗口显示登录界面
        emit windowClosed();
    }
}

void AdminWindow::onCloseClicked()
{
    int result = QMessageBox::question(this, "确认退出", "确定要关闭程序吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        QApplication::quit();
    }
}

AdminWindow::~AdminWindow()
{
} 