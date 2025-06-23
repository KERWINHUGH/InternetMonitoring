#include "AlarmDisplayWindow.h"
#include "ui_AlarmDisplayWindow.h"
#include "databasemanager.h"
#include <QDateTime>
#include <QHeaderView>

AlarmDisplayWindow::AlarmDisplayWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AlarmDisplayWindow)
{
    ui->setupUi(this);

    // 填充筛选器
    ui->deviceComboBox->addItem("所有设备", -1);
    QVariantList devices = DatabaseManager::instance().getDevices();
    for (const QVariant &deviceVariant : devices) {
        QVariantMap device = deviceVariant.toMap();
        ui->deviceComboBox->addItem(device["name"].toString(), device["device_id"]);
    }

    ui->statusComboBox->addItem("所有状态", "");
    ui->statusComboBox->addItem("未处理", "unprocessed");
    ui->statusComboBox->addItem("处理中", "processing");
    ui->statusComboBox->addItem("已解决", "resolved");

    // 设置时间范围为最近一天
    ui->endDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->startDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    
    // 初始化表格
    ui->recordTable->setColumnCount(7);
    ui->recordTable->setHorizontalHeaderLabels({"告警ID", "设备ID", "设备名称", "时间戳", "内容", "状态", "备注"});
    ui->recordTable->setColumnHidden(0, true);
    ui->recordTable->setColumnHidden(1, true);
    ui->recordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->recordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->recordTable->horizontalHeader()->setStretchLastSection(true);

    // 连接信号
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AlarmDisplayWindow::onFilterChanged);
    connect(ui->statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AlarmDisplayWindow::onFilterChanged);
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &AlarmDisplayWindow::onFilterChanged);
    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &AlarmDisplayWindow::onFilterChanged);
    connect(ui->recordTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        int currentRow = ui->recordTable->currentRow();
        if (currentRow < 0 || ui->recordTable->item(currentRow, 0) == nullptr) {
            ui->recordDetailText->clear();
            return;
        }
        
        QString id = ui->recordTable->item(currentRow, 0)->text();
        QString deviceId = ui->recordTable->item(currentRow, 1)->text();
        QString deviceName = ui->recordTable->item(currentRow, 2)->text();
        QString timestamp = ui->recordTable->item(currentRow, 3)->text();
        QString content = ui->recordTable->item(currentRow, 4)->text();
        QString status = ui->recordTable->item(currentRow, 5)->text();
        QString note = ui->recordTable->item(currentRow, 6)->text();

        QString details = QString("告警ID: \t%1\n"
                                  "设备ID: \t%2\n"
                                  "设备名称: \t%3\n"
                                  "告警时间: \t%4\n"
                                  "告警状态: \t%5\n"
                                  "告警内容: \t%6\n"
                                  "告警备注: \t%7")
                              .arg(id, deviceId, deviceName, timestamp, status, content, note);

        ui->recordDetailText->setText(details);
    });


    loadAlarms();
}

AlarmDisplayWindow::~AlarmDisplayWindow()
{
    delete ui;
}

void AlarmDisplayWindow::onFilterChanged()
{
    loadAlarms();
}

void AlarmDisplayWindow::loadAlarms()
{
    ui->recordTable->clearContents();
    ui->recordTable->setRowCount(0);

    // 从筛选器获取参数
    int deviceId = ui->deviceComboBox->currentData().toInt();
    QString status = ui->statusComboBox->currentData().toString();
    QDateTime startTime = ui->startDateTimeEdit->dateTime();
    QDateTime endTime = ui->endDateTimeEdit->dateTime();

    // 从数据库加载过滤后的告警记录
    QVariantList alarms = DatabaseManager::instance().getAlarmRecordsFiltered(deviceId, status, startTime, endTime);

    int row = 0;
    for (const QVariant &alarmVariant : alarms) {
        QVariantMap alarm = alarmVariant.toMap();

        ui->recordTable->insertRow(row);
        
        // 获取设备名称
        QVariantMap device;
        QString deviceName = "未知设备";
        if (DatabaseManager::instance().getDeviceById(alarm["device_id"].toInt(), device)) {
            deviceName = device["name"].toString();
        }

        QTableWidgetItem* idItem = new QTableWidgetItem(alarm["alarm_id"].toString());
        QTableWidgetItem* deviceIdItem = new QTableWidgetItem(alarm["device_id"].toString());
        QTableWidgetItem* deviceNameItem = new QTableWidgetItem(deviceName);
        QTableWidgetItem* timestampItem = new QTableWidgetItem(alarm["timestamp"].toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        QTableWidgetItem* contentItem = new QTableWidgetItem(alarm["content"].toString());
        QTableWidgetItem* statusItem = new QTableWidgetItem(alarm["status"].toString());
        QTableWidgetItem* noteItem = new QTableWidgetItem(alarm["note"].toString());

        ui->recordTable->setItem(row, 0, idItem);
        ui->recordTable->setItem(row, 1, deviceIdItem);
        ui->recordTable->setItem(row, 2, deviceNameItem);
        ui->recordTable->setItem(row, 3, timestampItem);
        ui->recordTable->setItem(row, 4, contentItem);
        ui->recordTable->setItem(row, 5, statusItem);
        ui->recordTable->setItem(row, 6, noteItem);
        
        row++;
    }

    ui->recordDetailText->clear();
} 