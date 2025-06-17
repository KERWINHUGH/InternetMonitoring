#include "userwindow.h"
#include "ui_userwindow.h"
#include <QApplication>
#include <QAction>

UserWindow::UserWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UserWindow)
{
    ui->setupUi(this);
    setFixedSize(800, 600);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // 信号槽连接
    connect(ui->networkMonitorBtn, &QPushButton::clicked, this, &UserWindow::onMonitoringClicked);
    connect(ui->personalSettingsBtn, &QPushButton::clicked, this, &UserWindow::onPersonalSettingsClicked);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &UserWindow::onLogoutClicked);
    connect(ui->closeBtn, &QPushButton::clicked, this, &UserWindow::onCloseClicked);
    // 菜单栏
    connect(ui->action_logout, &QAction::triggered, this, &UserWindow::onLogoutClicked);
    connect(ui->action_exit, &QAction::triggered, this, &UserWindow::onCloseClicked);
    connect(ui->action_monitor, &QAction::triggered, this, &UserWindow::onMonitoringClicked);
    connect(ui->action_settings, &QAction::triggered, this, &UserWindow::onPersonalSettingsClicked);
    connect(ui->action_about, &QAction::triggered, [this]() {
        QMessageBox::about(this, "关于", "用户控制台 v1.0\n\n网络监控系统用户界面");
    });
    // 状态栏
    statusBar()->showMessage("用户控制台已就绪");
}

void UserWindow::onMonitoringClicked()
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
    delete ui;
} 
