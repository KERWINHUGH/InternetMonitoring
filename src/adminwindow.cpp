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
#include <QButtonGroup>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

// 用户管理页面集成DatabaseViewer
class UserManagementPage : public QWidget {
public:
    UserManagementPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        DatabaseViewer* viewer = new DatabaseViewer(this, QStringList() << "users" << "device_groups" << "devices");
        viewer->setWindowFlags(Qt::Widget);
        layout->addWidget(viewer);
        layout->setContentsMargins(0,0,0,0);
    }
};

// 设备管理页面集成DeviceManagementWindow
class DeviceManagementPage : public QWidget {
public:
    DeviceManagementPage(const QString& currentUsername, QWidget* parent = nullptr)
        : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        DeviceManagementWindow* devWin = new DeviceManagementWindow(currentUsername, true, this);
        devWin->setWindowFlags(Qt::Widget);
        layout->addWidget(devWin);
        layout->setContentsMargins(0,0,0,0);
    }
};

// 系统设置页面（可后续扩展实际设置内容）
class SystemSettingsPage : public QWidget {
public:
    SystemSettingsPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        QLabel* label = new QLabel("系统设置区（可集成实际设置UI）", this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size:20px;color:#1976D2;font-weight:bold;");
        layout->addWidget(label);
        layout->setContentsMargins(0,0,0,0);
    }
};

class NetworkMonitorPage : public QWidget {
public:
    NetworkMonitorPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        NetworkMonitorWindow* win = new NetworkMonitorWindow(this);
        win->setWindowFlags(Qt::Widget);
        layout->addWidget(win);
        layout->setContentsMargins(0,0,0,0);
    }
};
class AlarmRuleManagementPage : public QWidget {
public:
    AlarmRuleManagementPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        AlarmRuleManagementWindow* win = new AlarmRuleManagementWindow(this);
        win->setWindowFlags(Qt::Widget);
        layout->addWidget(win);
        layout->setContentsMargins(0,0,0,0);
    }
};
class AlarmDisplayPage : public QWidget {
public:
    AlarmDisplayPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        AlarmDisplayWindow* win = new AlarmDisplayWindow(this);
        win->setWindowFlags(Qt::Widget);
        layout->addWidget(win);
        layout->setContentsMargins(0,0,0,0);
    }
};
class DataAnalysisPage : public QWidget {
public:
    DataAnalysisPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        DataAnalysisWindow* win = new DataAnalysisWindow(this);
        win->setWindowFlags(Qt::Widget);
        layout->addWidget(win);
        layout->setContentsMargins(0,0,0,0);
    }
};

AdminWindow::AdminWindow(const QString& currentUsername, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminWindow), databaseViewer(nullptr), deviceManagementWindow(nullptr),
      networkMonitorWindow(nullptr), alarmRuleManagementWindow(nullptr), alarmDisplayWindow(nullptr), dataAnalysisWindow(nullptr),
      currentUsername(currentUsername)
{
    ui->setupUi(this);
    setFixedSize(800, 600);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    ui->systemSettingsBtn->hide();

    // 全局QSS美化
    QString mainQss = R"(
    QWidget {
        background: #f5f6fa;
        font-family: 'Microsoft YaHei', '微软雅黑', sans-serif;
        font-size: 16px;
    }
    QLabel[role='title'] {
        font-size: 22px;
        font-weight: bold;
        color: #1565C0;
        qproperty-alignment: 'AlignCenter';
        margin: 24px 0 16px 0;
    }
    QTableWidget, QLineEdit, QComboBox {
        border-radius: 8px;
        border: 1px solid #cfd8dc;
        background: #fff;
    }
    QPushButton, QToolButton {
        border-radius: 8px;
        border: none;
        font-size: 16px;
    }
    QPushButton:hover, QToolButton:hover {
        background: #1976D2;
        color: #fff;
    }
    )";
    qApp->setStyleSheet(mainQss);

    // 设置QSS样式
    QString sideBarQss = R"(
    QToolButton {
        background: transparent;
        color: #bbb;
        border: none;
        padding: 16px 0;
        font-size: 15px;
    }
    QToolButton:checked, QToolButton:hover {
        background: #2196F3;
        color: #fff;
        border-radius: 8px;
    }
    QPushButton#logoutBtn, QPushButton#exitBtn {
        background: #f44336;
        color: #fff;
        border-radius: 8px;
        margin: 8px 0;
        padding: 10px 0;
    }
    )";
    ui->sideBarWidget->setStyleSheet(sideBarQss);

    // 按钮与页面切换
    QList<QToolButton*> btns = {ui->userManagementBtn, ui->deviceManagementBtn, ui->networkMonitorBtn, ui->alarmRuleManagementBtn, ui->alarmDisplayBtn, ui->dataAnalysisBtn};
    for (int i = 0; i < btns.size(); ++i) {
        connect(btns[i], &QToolButton::clicked, this, [=]() {
            smoothSwitchToPage(i);
        });
    }

    // 信号槽连接
    connect(ui->logoutBtn, &QPushButton::clicked, this, &AdminWindow::onLogoutClicked);
    connect(ui->exitBtn, &QPushButton::clicked, this, &AdminWindow::onExitClicked);
    // 菜单栏（已移除，无需连接action_*）
    // 状态栏（如无statusBar成员可注释）
    // statusBar()->showMessage("管理员控制台已就绪");

    // 实例化功能页面并添加到QStackedWidget
    UserManagementPage* userPage = new UserManagementPage(this);
    DeviceManagementPage* devicePage = new DeviceManagementPage(currentUsername, this);
    NetworkMonitorPage* networkPage = new NetworkMonitorPage(this);
    AlarmRuleManagementPage* alarmRulePage = new AlarmRuleManagementPage(this);
    AlarmDisplayPage* alarmDisplayPage = new AlarmDisplayPage(this);
    DataAnalysisPage* dataAnalysisPage = new DataAnalysisPage(this);

    ui->mainStackedWidget->addWidget(userPage);         // index 0
    ui->mainStackedWidget->addWidget(devicePage);       // index 1
    ui->mainStackedWidget->addWidget(networkPage);      // index 2
    ui->mainStackedWidget->addWidget(alarmRulePage);    // index 3
    ui->mainStackedWidget->addWidget(alarmDisplayPage); // index 4
    ui->mainStackedWidget->addWidget(dataAnalysisPage); // index 5

    // 设置默认显示用户管理页面
    ui->mainStackedWidget->setCurrentIndex(0);
    ui->userManagementBtn->setChecked(true);
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

void AdminWindow::onLogoutClicked()
{
    int result = QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        this->close();
        emit windowClosed();
    }
}

void AdminWindow::onExitClicked()
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

void AdminWindow::smoothSwitchToPage(int pageIndex) {
    QWidget* current = ui->mainStackedWidget->currentWidget();
    QWidget* next = ui->mainStackedWidget->widget(pageIndex);
    if (current == next) return;
    int w = ui->mainStackedWidget->width();
    next->setGeometry(w, 0, next->width(), next->height());
    next->show();
    QPropertyAnimation* animOut = new QPropertyAnimation(current, "geometry");
    animOut->setDuration(300);
    animOut->setStartValue(current->geometry());
    animOut->setEndValue(QRect(-w, 0, current->width(), current->height()));
    QPropertyAnimation* animIn = new QPropertyAnimation(next, "geometry");
    animIn->setDuration(300);
    animIn->setStartValue(QRect(w, 0, next->width(), next->height()));
    animIn->setEndValue(QRect(0, 0, next->width(), next->height()));
    QParallelAnimationGroup* group = new QParallelAnimationGroup;
    group->addAnimation(animOut);
    group->addAnimation(animIn);
    connect(group, &QParallelAnimationGroup::finished, this, [=]() {
        ui->mainStackedWidget->setCurrentIndex(pageIndex);
        current->hide();
        next->setGeometry(0, 0, next->width(), next->height());
        group->deleteLater();
    });
    group->start();
}
