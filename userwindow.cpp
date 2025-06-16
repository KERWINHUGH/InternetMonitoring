#include "userwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>

UserWindow::UserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createMenuBar();
    
    // 设置窗口属性
    setWindowTitle("用户控制台");
    setFixedSize(800, 600);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
}

void UserWindow::setupUI()
{
    // 创建中央部件
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    
    // 欢迎标签
    welcomeLabel = new QLabel("欢迎使用网络监控系统", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #27ae60; margin: 20px; }");
    mainLayout->addWidget(welcomeLabel);
    
    // 功能按钮
    networkMonitorBtn = new QPushButton("网络监控", this);
    networkMonitorBtn->setFixedSize(200, 50);
    networkMonitorBtn->setStyleSheet(
        "QPushButton { "
        "    background-color: #27ae60; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #229954; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #1e8449; "
        "}"
    );
    
    personalSettingsBtn = new QPushButton("个人设置", this);
    personalSettingsBtn->setFixedSize(200, 50);
    personalSettingsBtn->setStyleSheet(
        "QPushButton { "
        "    background-color: #f39c12; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #e67e22; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #d35400; "
        "}"
    );
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(networkMonitorBtn);
    buttonLayout->addWidget(personalSettingsBtn);
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
    connect(networkMonitorBtn, &QPushButton::clicked, this, &UserWindow::onNetworkMonitorClicked);
    connect(personalSettingsBtn, &QPushButton::clicked, this, &UserWindow::onPersonalSettingsClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &UserWindow::onLogoutClicked);
    connect(closeBtn, &QPushButton::clicked, this, &UserWindow::onCloseClicked);
    
    // 添加状态栏
    statusBar()->showMessage("用户控制台已就绪");
}

void UserWindow::createMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件");
    QAction *logoutAction = fileMenu->addAction("退出登录");
    QAction *exitAction = fileMenu->addAction("退出程序");
    
    // 功能菜单
    QMenu *functionMenu = menuBar->addMenu("功能");
    QAction *monitorAction = functionMenu->addAction("网络监控");
    QAction *settingsAction = functionMenu->addAction("个人设置");
    
    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助");
    QAction *aboutAction = helpMenu->addAction("关于");
    
    // 连接菜单信号
    connect(logoutAction, &QAction::triggered, this, &UserWindow::onLogoutClicked);
    connect(exitAction, &QAction::triggered, this, &UserWindow::onCloseClicked);
    connect(monitorAction, &QAction::triggered, this, &UserWindow::onNetworkMonitorClicked);
    connect(settingsAction, &QAction::triggered, this, &UserWindow::onPersonalSettingsClicked);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "关于", "用户控制台 v1.0\n\n网络监控系统用户界面");
    });
}

void UserWindow::onNetworkMonitorClicked()
{
    QMessageBox::information(this, "网络监控", "网络监控功能正在开发中...");
}

void UserWindow::onPersonalSettingsClicked()
{
    QMessageBox::information(this, "个人设置", "个人设置功能正在开发中...");
}

void UserWindow::onLogoutClicked()
{
    int result = QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        this->close();
        // 发送信号通知主窗口显示登录界面
        emit windowClosed();
    }
}

void UserWindow::onCloseClicked()
{
    int result = QMessageBox::question(this, "确认退出", "确定要关闭程序吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        QApplication::quit();
    }
}

UserWindow::~UserWindow()
{
} 