#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QApplication>
#include <QAction>
#include <QMessageBox>
#include "databaseviewer.h"
#include "DeviceManagementWindow.h"
#include "NetworkMonitorWindow.h"
#include "AlarmRuleManagementWindow.h"
#include "AlarmDisplayWindow.h"
#include "DataAnalysisWindow.h"

AdminWindow::AdminWindow(const QString& currentUsername, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminWindow), databaseViewer(nullptr), deviceManagementWindow(nullptr),
      networkMonitorWindow(nullptr), alarmRuleManagementWindow(nullptr), alarmDisplayWindow(nullptr), dataAnalysisWindow(nullptr),
      currentUsername(currentUsername)
{
    ui->setupUi(this);
    setFixedSize(800, 600);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // 信号槽连接
    connect(ui->userManagementBtn, &QPushButton::clicked, this, &AdminWindow::onUserManagementClicked);
    connect(ui->systemSettingsBtn, &QPushButton::clicked, this, &AdminWindow::onSystemSettingsClicked);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &AdminWindow::onLogoutClicked);
    connect(ui->closeBtn, &QPushButton::clicked, this, &AdminWindow::onCloseClicked);
    connect(ui->deviceManagementBtn, &QPushButton::clicked, this, &AdminWindow::onDeviceManagementClicked);
    connect(ui->networkMonitorBtn, &QPushButton::clicked, this, &AdminWindow::onNetworkMonitorClicked);
    connect(ui->alarmRuleManagementBtn, &QPushButton::clicked, this, &AdminWindow::onAlarmRuleManagementClicked);
    connect(ui->alarmDisplayBtn, &QPushButton::clicked, this, &AdminWindow::onAlarmDisplayClicked);
    connect(ui->dataAnalysisBtn, &QPushButton::clicked, this, &AdminWindow::onDataAnalysisClicked);
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
    if (databaseViewer) {
        delete databaseViewer;
    }
    if (deviceManagementWindow) {
        delete deviceManagementWindow;
    }
    if (networkMonitorWindow) {
        delete networkMonitorWindow;
    }
    if (alarmRuleManagementWindow) {
        delete alarmRuleManagementWindow;
    }
    if (alarmDisplayWindow) {
        delete alarmDisplayWindow;
    }
    if (dataAnalysisWindow) {
        delete dataAnalysisWindow;
    }
}

void AdminWindow::onUserManagementClicked()
{
    if (!databaseViewer) {
        databaseViewer = new DatabaseViewer(this, QStringList() << "users");
    }
    databaseViewer->show();
    databaseViewer->raise();
    databaseViewer->activateWindow();
}

void AdminWindow::onSystemSettingsClicked()
{
    if (!databaseViewer) {
        databaseViewer = new DatabaseViewer(this);
    }
    databaseViewer->show();
    databaseViewer->raise();
    databaseViewer->activateWindow();
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

void AdminWindow::onDeviceManagementClicked()
{
    if (!deviceManagementWindow) {
        deviceManagementWindow = new DeviceManagementWindow(currentUsername, true, this);
    }
    deviceManagementWindow->show();
    deviceManagementWindow->raise();
    deviceManagementWindow->activateWindow();
}

void AdminWindow::onNetworkMonitorClicked()
{
    if (!networkMonitorWindow) {
        networkMonitorWindow = new NetworkMonitorWindow(this);
    }
    networkMonitorWindow->show();
    networkMonitorWindow->raise();
    networkMonitorWindow->activateWindow();
}

void AdminWindow::onAlarmRuleManagementClicked()
{
    if (!alarmRuleManagementWindow) {
        alarmRuleManagementWindow = new AlarmRuleManagementWindow(this);
    }
    alarmRuleManagementWindow->show();
    alarmRuleManagementWindow->raise();
    alarmRuleManagementWindow->activateWindow();
}

void AdminWindow::onAlarmDisplayClicked()
{
    if (!alarmDisplayWindow) {
        alarmDisplayWindow = new AlarmDisplayWindow(this);
    }
    alarmDisplayWindow->show();
    alarmDisplayWindow->raise();
    alarmDisplayWindow->activateWindow();
}

void AdminWindow::onDataAnalysisClicked()
{
    if (!dataAnalysisWindow) {
        dataAnalysisWindow = new DataAnalysisWindow(this);
    }
    dataAnalysisWindow->show();
    dataAnalysisWindow->raise();
    dataAnalysisWindow->activateWindow();
} 