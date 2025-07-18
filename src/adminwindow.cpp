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
#include <QIcon>
#include <QStyle>

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

// 新增：系统日志页面
class SystemLogsPage : public QWidget {
public:
    SystemLogsPage(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        DatabaseViewer* viewer = new DatabaseViewer(this, QStringList() << "system_logs");
        viewer->setWindowFlags(Qt::Widget);
        layout->addWidget(viewer);
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
      currentUsername(currentUsername), sideBarGroup(nullptr)
{
    ui->setupUi(this);
    // setFixedSize(900, 650); // 移除固定大小限制，使其可缩放
    setMinimumSize(900, 650); // 设置一个最小尺寸以保证布局不会错乱
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    // 复用系统设置按钮作为系统日志按钮
    ui->systemSettingsBtn->setText("系统日志");
    ui->systemSettingsBtn->show();

    // 优化：为按钮添加图标并设置样式
    ui->sideBarWidget->setMinimumWidth(160);

    // 按钮顺序需要和UI文件以及页面索引严格对应
    QList<QToolButton*> btns = {
        ui->userManagementBtn,      // 0
        ui->deviceManagementBtn,    // 1
        ui->networkMonitorBtn,      // 2
        ui->alarmRuleManagementBtn, // 3
        ui->alarmDisplayBtn,        // 4
        ui->dataAnalysisBtn,        // 5
        ui->systemSettingsBtn       // 6 -> 现在在数据分析下方
    };
    QList<QStyle::StandardPixmap> icons = {
        QStyle::SP_DirHomeIcon,           // 用户管理
        QStyle::SP_ComputerIcon,          // 设备管理
        QStyle::SP_DriveNetIcon,          // 网络监控
        QStyle::SP_FileIcon,              // 报警规则
        QStyle::SP_MessageBoxWarning,     // 报警显示
        QStyle::SP_FileDialogDetailedView, // 数据分析
        QStyle::SP_DialogApplyButton      // 系统日志
    };

    sideBarGroup = new QButtonGroup(this);

    for (int i = 0; i < btns.size(); ++i) {
        btns[i]->setIcon(style()->standardIcon(icons[i]));
        btns[i]->setIconSize(QSize(22, 22));
        btns[i]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        btns[i]->setMinimumHeight(40);
        btns[i]->setCheckable(true); // 设置为可勾选
        sideBarGroup->addButton(btns[i], i); // 添加到按钮组
    }
    sideBarGroup->setExclusive(true); // 设置互斥

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
    #sideBarWidget {
        background: #2c3e50; /* 设置侧边栏背景色 */
        border-right: 1px solid #34495e;
    }
    QToolButton {
        background: transparent;
        color: #bdc3c7; /* 默认文字颜色 */
        border: none;
        padding: 10px 15px;
        text-align: left;
        font-size: 14px;
        font-weight: normal;
    }
    QToolButton:hover {
        background: #34495e; /* 悬停背景色 */
        color: white;
    }
    QToolButton:checked {
        color: white;
        font-weight: bold;
        background: #2980b9; /* 选中背景色 */
        border-left: 3px solid #3498db; /* 选中指示条 */
    }
    QPushButton#logoutBtn, QPushButton#exitBtn {
        background: #f44336;
        color: #fff;
        border-radius: 8px;
        margin: 8px 15px; /* 调整边距 */
        padding: 10px 0;
    }
    QPushButton#logoutBtn:hover, QPushButton#exitBtn:hover {
        background: #e53935; /* 悬停时颜色加深 */
    }
    )";
    ui->sideBarWidget->setStyleSheet(sideBarQss);

    // 按钮与页面切换
    connect(sideBarGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &AdminWindow::smoothSwitchToPage);

    // 将各个按钮的点击事件连接到对应的槽函数
    connect(ui->userManagementBtn, &QToolButton::clicked, this, &AdminWindow::onUserManagementClicked);
    connect(ui->deviceManagementBtn, &QToolButton::clicked, this, &AdminWindow::onDeviceManagementClicked);
    connect(ui->networkMonitorBtn, &QToolButton::clicked, this, &AdminWindow::onNetworkMonitorClicked);
    connect(ui->alarmRuleManagementBtn, &QToolButton::clicked, this, &AdminWindow::onAlarmRuleManagementClicked);
    connect(ui->alarmDisplayBtn, &QToolButton::clicked, this, &AdminWindow::onAlarmDisplayClicked);
    connect(ui->dataAnalysisBtn, &QToolButton::clicked, this, &AdminWindow::onDataAnalysisClicked);

    // 信号槽连接
    connect(ui->logoutBtn, &QPushButton::clicked, this, &AdminWindow::onLogoutClicked);
    connect(ui->exitBtn, &QPushButton::clicked, this, &AdminWindow::onExitClicked);
    // 菜单栏（已移除，无需连接action_*）
    // 状态栏（如无statusBar成员可注释）
    // statusBar()->showMessage("管理员控制台已就绪");

    // 实例化功能页面并添加到QStackedWidget
    DatabaseViewer* userPage = new DatabaseViewer(this, QStringList() << "users");
    DeviceManagementPage* devicePage = new DeviceManagementPage(currentUsername, this);
    NetworkMonitorPage* networkPage = new NetworkMonitorPage(this);
    AlarmRuleManagementPage* alarmRulePage = new AlarmRuleManagementPage(this);
    AlarmDisplayPage* alarmDisplayPage = new AlarmDisplayPage(this);
    DataAnalysisPage* dataAnalysisPage = new DataAnalysisPage(this);
    SystemLogsPage* logsPage = new SystemLogsPage(this);

    ui->mainStackedWidget->addWidget(userPage);         // index 0
    ui->mainStackedWidget->addWidget(devicePage);       // index 1
    ui->mainStackedWidget->addWidget(networkPage);      // index 2
    ui->mainStackedWidget->addWidget(alarmRulePage);    // index 3
    ui->mainStackedWidget->addWidget(alarmDisplayPage); // index 4
    ui->mainStackedWidget->addWidget(dataAnalysisPage); // index 5
    ui->mainStackedWidget->addWidget(logsPage);         // index 6

    // 设置默认显示设备管理页面
    ui->deviceManagementBtn->click();
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
    ui->mainStackedWidget->setCurrentIndex(0);
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
    ui->mainStackedWidget->setCurrentIndex(1);
}

void AdminWindow::onNetworkMonitorClicked()
{
    ui->mainStackedWidget->setCurrentIndex(2);
}

void AdminWindow::onAlarmRuleManagementClicked()
{
    ui->mainStackedWidget->setCurrentIndex(3);
}

void AdminWindow::onAlarmDisplayClicked()
{
    ui->mainStackedWidget->setCurrentIndex(4);
}

void AdminWindow::onDataAnalysisClicked()
{
    ui->mainStackedWidget->setCurrentIndex(5);
}

void AdminWindow::smoothSwitchToPage(int pageIndex) {
    if (ui->mainStackedWidget->currentIndex() == pageIndex) return;

    QWidget* currentPage = ui->mainStackedWidget->widget(ui->mainStackedWidget->currentIndex());
    QWidget* nextPage = ui->mainStackedWidget->widget(pageIndex);

    if (!currentPage || !nextPage) return;

    // 创建淡出动画
    QPropertyAnimation* fadeOut = new QPropertyAnimation(currentPage, "windowOpacity");
    fadeOut->setDuration(150);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    connect(fadeOut, &QPropertyAnimation::finished, [=]() {
        ui->mainStackedWidget->setCurrentIndex(pageIndex);
    });

    // 创建淡入动画
    QPropertyAnimation* fadeIn = new QPropertyAnimation(nextPage, "windowOpacity");
    fadeIn->setDuration(150);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(fadeOut);
    group->addAnimation(fadeIn);

    connect(group, &QParallelAnimationGroup::finished, [group](){
        delete group; // 动画结束后清理
    });

    group->start();
}

void AdminWindow::closeEvent(QCloseEvent *event) {
    onExitClicked();
    event->ignore(); // 阻止窗口立即关闭，由onExitClicked中的逻辑决定
}
