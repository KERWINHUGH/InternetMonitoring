#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QApplication>
#include <QAction>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setFixedSize(800, 600);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // 信号槽连接
    connect(ui->userManagementBtn, &QPushButton::clicked, this, &AdminWindow::onUserManagementClicked);
    connect(ui->systemSettingsBtn, &QPushButton::clicked, this, &AdminWindow::onSystemSettingsClicked);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &AdminWindow::onLogoutClicked);
    connect(ui->closeBtn, &QPushButton::clicked, this, &AdminWindow::onCloseClicked);
    // 菜单栏
    connect(ui->action_logout, &QAction::triggered, this, &AdminWindow::onLogoutClicked);
    connect(ui->action_exit, &QAction::triggered, this, &AdminWindow::onCloseClicked);
    connect(ui->action_user, &QAction::triggered, this, &AdminWindow::onUserManagementClicked);
    connect(ui->action_settings, &QAction::triggered, this, &AdminWindow::onSystemSettingsClicked);
    connect(ui->action_about, &QAction::triggered, [this]() {
        QMessageBox::about(this, "关于", "管理员控制台 v1.0\n\n网络监控系统管理员界面");
    });
    // 状态栏
    statusBar()->showMessage("管理员控制台已就绪");
}

AdminWindow::~AdminWindow()
{
    delete ui;
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