#include "userwindow.h"
#include "ui_userwindow.h"
#include "profilewindow.h"
#include "databaseviewer.h"
#include <QApplication>
#include <QAction>
#include <QMessageBox>
#include <QButtonGroup>
#include <QStyle>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QPushButton>

// 只读功能页
class ReadOnlyDatabasePage : public QWidget {
public:
    ReadOnlyDatabasePage(const QString& table, QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        DatabaseViewer* viewer = new DatabaseViewer(this, QStringList() << table, true); // 只读
        viewer->setWindowFlags(Qt::Widget);
        layout->addWidget(viewer);
        layout->setContentsMargins(0,0,0,0);
    }
};

UserWindow::UserWindow(const QString& username, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UserWindow), currentUsername(username), profileWindow(nullptr)
{
    ui->setupUi(this);
    setMinimumSize(900, 650);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    // 主布局
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    setCentralWidget(central);

    // 侧边栏
    sideBarWidget = new QWidget(this);
    QVBoxLayout* sideBarLayout = new QVBoxLayout(sideBarWidget);
    sideBarWidget->setMinimumWidth(160);

    deviceManagementBtn = new QToolButton(this);
    deviceManagementBtn->setText("设备管理");
    networkMonitorBtn = new QToolButton(this);
    networkMonitorBtn->setText("网络监控");
    alarmRuleManagementBtn = new QToolButton(this);
    alarmRuleManagementBtn->setText("告警规则");
    alarmDisplayBtn = new QToolButton(this);
    alarmDisplayBtn->setText("告警展示");
    dataAnalysisBtn = new QToolButton(this);
    dataAnalysisBtn->setText("数据分析");
    profileBtn = new QToolButton(this);
    profileBtn->setText("个人设置");
    logoutBtn = new QPushButton("登出", this);
    exitBtn = new QPushButton("退出", this);

    QList<QToolButton*> btns = {deviceManagementBtn, networkMonitorBtn, alarmRuleManagementBtn, alarmDisplayBtn, dataAnalysisBtn, profileBtn};
    QList<QStyle::StandardPixmap> icons = {
        QStyle::SP_ComputerIcon,
        QStyle::SP_DriveNetIcon,
        QStyle::SP_FileIcon,
        QStyle::SP_MessageBoxWarning,
        QStyle::SP_FileDialogDetailedView,
        QStyle::SP_DirHomeIcon
    };
    sideBarGroup = new QButtonGroup(this);
    for (int i = 0; i < btns.size(); ++i) {
        btns[i]->setIcon(style()->standardIcon(icons[i]));
        btns[i]->setIconSize(QSize(22, 22));
        btns[i]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        btns[i]->setMinimumHeight(40);
        btns[i]->setCheckable(true);
        sideBarGroup->addButton(btns[i], i);
        sideBarLayout->addWidget(btns[i]);
    }
    sideBarLayout->addStretch();
    sideBarLayout->addWidget(logoutBtn);
    sideBarLayout->addWidget(exitBtn);
    mainLayout->addWidget(sideBarWidget);

    // 主内容区
    mainStackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(mainStackedWidget);

    // 只读功能页
    mainStackedWidget->addWidget(new ReadOnlyDatabasePage("devices", this));        // 0 设备管理
    mainStackedWidget->addWidget(new ReadOnlyDatabasePage("monitor_data", this));   // 1 网络监控
    mainStackedWidget->addWidget(new ReadOnlyDatabasePage("alarm_rules", this));    // 2 告警规则
    mainStackedWidget->addWidget(new ReadOnlyDatabasePage("alarm_records", this));  // 3 告警展示
    mainStackedWidget->addWidget(new ReadOnlyDatabasePage("monitor_data", this));   // 4 数据分析
    // 个人设置页
    profileWindow = new ProfileWindow(currentUsername, this);
    mainStackedWidget->addWidget(profileWindow);                                      // 5 个人设置

    // QSS风格同步
    QString sideBarQss = R"(
    #sideBarWidget {
        background: #2c3e50;
        border-right: 1px solid #34495e;
    }
    QToolButton {
        background: transparent;
        color: #bdc3c7;
        border: none;
        padding: 10px 15px;
        text-align: left;
        font-size: 14px;
        font-weight: normal;
    }
    QToolButton:hover {
        background: #34495e;
        color: white;
    }
    QToolButton:checked {
        color: white;
        font-weight: bold;
        background: #2980b9;
        border-left: 3px solid #3498db;
    }
    QPushButton#logoutBtn, QPushButton#exitBtn {
        background: #f44336;
        color: #fff;
        border-radius: 8px;
        margin: 8px 15px;
        padding: 10px 0;
    }
    QPushButton#logoutBtn:hover, QPushButton#exitBtn:hover {
        background: #e53935;
    }
    )";
    sideBarWidget->setStyleSheet(sideBarQss);

    // 信号槽
    connect(sideBarGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &UserWindow::onSidebarButtonClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &UserWindow::onLogoutClicked);
    connect(exitBtn, &QPushButton::clicked, this, &UserWindow::onExitClicked);
    connect(profileWindow, &ProfileWindow::profileUpdated, this, &UserWindow::onProfileUpdated);

    // 默认显示设备管理
    deviceManagementBtn->click();
}

UserWindow::~UserWindow()
{
    delete ui;
}

void UserWindow::onSidebarButtonClicked(int index)
{
    mainStackedWidget->setCurrentIndex(index);
    QList<QToolButton*> btns = {deviceManagementBtn, networkMonitorBtn, alarmRuleManagementBtn, alarmDisplayBtn, dataAnalysisBtn, profileBtn};
    for (int i = 0; i < btns.size(); ++i) {
        btns[i]->setChecked(i == index);
    }
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

void UserWindow::onExitClicked()
{
    int result = QMessageBox::question(this, "确认退出", "确定要关闭程序吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void UserWindow::onProfileUpdated()
{
    QMessageBox::information(this, "成功", "个人信息已更新");
} 
