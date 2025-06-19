#include "NetworkMonitorWindow.h"
#include "ui_NetworkMonitorWindow.h"
#include <QTimer>

NetworkMonitorWindow::NetworkMonitorWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::NetworkMonitorWindow), realtimeTimer(new QTimer(this))
{
    ui->setupUi(this);
    setupConnections();
    // 启动定时器，定时刷新实时数据
    connect(realtimeTimer, &QTimer::timeout, this, &NetworkMonitorWindow::refreshRealtimeData);
    realtimeTimer->start(2000); // 2秒刷新一次
}

NetworkMonitorWindow::~NetworkMonitorWindow()
{
    delete ui;
}

void NetworkMonitorWindow::setupConnections()
{
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NetworkMonitorWindow::onDeviceChanged);
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &NetworkMonitorWindow::onTimeRangeChanged);
    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &NetworkMonitorWindow::onTimeRangeChanged);
    connect(ui->exportButton, &QPushButton::clicked, this, &NetworkMonitorWindow::onExportClicked);
}

void NetworkMonitorWindow::onDeviceChanged(int index) { /* TODO: 切换设备 */ }
void NetworkMonitorWindow::onTimeRangeChanged() { /* TODO: 查询历史数据 */ }
void NetworkMonitorWindow::onExportClicked() { /* TODO: 导出数据 */ }
void NetworkMonitorWindow::refreshRealtimeData() { /* TODO: 刷新实时数据 */ }
void NetworkMonitorWindow::queryHistoryData() { /* TODO: 查询历史数据 */ }
void NetworkMonitorWindow::updateRealtimeChart() { /* TODO: 更新实时图表 */ }
void NetworkMonitorWindow::updateHistoryTable() { /* TODO: 更新历史表格 */ }
void NetworkMonitorWindow::updateHistoryChart() { /* TODO: 更新历史图表 */ } 