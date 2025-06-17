#include "databaseviewer.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseViewer::DatabaseViewer(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    loadTableData("users"); // 默认显示用户表
}

DatabaseViewer::~DatabaseViewer()
{
}

void DatabaseViewer::setupUI()
{
    setWindowTitle("数据库查看器");
    setFixedSize(800, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    controlLayout = new QHBoxLayout();

    // 创建控件
    QLabel *tableLabel = new QLabel("选择表:", this);
    tableComboBox = new QComboBox(this);
    tableComboBox->addItems({"users", "devices", "monitor_data", "alarms", "system_logs"});

    refreshButton = new QPushButton("刷新", this);
    exportButton = new QPushButton("导出", this);
    statusLabel = new QLabel("就绪", this);

    // 添加到控制布局
    controlLayout->addWidget(tableLabel);
    controlLayout->addWidget(tableComboBox);
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(exportButton);
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);

    // 创建数据表格
    dataTable = new QTableWidget(this);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);

    // 添加到主布局
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(dataTable);

    // 连接信号槽
    connect(tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DatabaseViewer::onTableChanged);
    connect(refreshButton, &QPushButton::clicked, this, &DatabaseViewer::onRefreshClicked);
    connect(exportButton, &QPushButton::clicked, this, &DatabaseViewer::onExportClicked);
}

void DatabaseViewer::onTableChanged(int index)
{
    QString tableName = tableComboBox->currentText();
    loadTableData(tableName);
}

void DatabaseViewer::onRefreshClicked()
{
    QString tableName = tableComboBox->currentText();
    loadTableData(tableName);
}

void DatabaseViewer::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据", 
                                                   "database_export.csv", 
                                                   "CSV文件 (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    // 写入表头
    QStringList headers;
    for (int i = 0; i < dataTable->columnCount(); ++i) {
        headers << dataTable->horizontalHeaderItem(i)->text();
    }
    out << headers.join(",") << "\n";

    // 写入数据
    for (int row = 0; row < dataTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < dataTable->columnCount(); ++col) {
            QTableWidgetItem *item = dataTable->item(row, col);
            rowData << (item ? item->text() : "");
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "数据已导出到: " + fileName);
}

void DatabaseViewer::loadTableData(const QString& tableName)
{
    statusLabel->setText("正在加载数据...");

    if (tableName == "users") {
        displayUsers();
    } else if (tableName == "devices") {
        displayDevices();
    } else if (tableName == "monitor_data") {
        displayMonitorData();
    } else if (tableName == "alarms") {
        displayAlarms();
    } else if (tableName == "system_logs") {
        displaySystemLogs();
    }

    statusLabel->setText(QString("已加载 %1 条记录").arg(dataTable->rowCount()));
}

void DatabaseViewer::displayUsers()
{
    dataTable->clear();
    dataTable->setColumnCount(6);
    dataTable->setHorizontalHeaderLabels({"用户名", "密码", "邮箱", "手机号", "昵称", "角色"});

    QSqlQuery query("SELECT username, password, email, phone, nickname, role FROM users");
    
    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 6; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            dataTable->setItem(row, col, item);
        }
        row++;
    }
}

void DatabaseViewer::displayDevices()
{
    dataTable->clear();
    dataTable->setColumnCount(5);
    dataTable->setHorizontalHeaderLabels({"设备ID", "名称", "类型", "位置", "状态"});

    QSqlQuery query("SELECT device_id, name, type, location, status FROM devices");
    
    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            dataTable->setItem(row, col, item);
        }
        row++;
    }
}

void DatabaseViewer::displayMonitorData()
{
    dataTable->clear();
    dataTable->setColumnCount(8);
    dataTable->setHorizontalHeaderLabels({"ID", "设备ID", "时间戳", "温度", "湿度", "CPU使用率", "内存使用率", "网络速度"});

    QSqlQuery query("SELECT id, device_id, timestamp, temperature, humidity, cpu_usage, memory_usage, network_speed FROM monitor_data ORDER BY timestamp DESC LIMIT 100");
    
    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 8; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            dataTable->setItem(row, col, item);
        }
        row++;
    }
}

void DatabaseViewer::displayAlarms()
{
    dataTable->clear();
    dataTable->setColumnCount(7);
    dataTable->setHorizontalHeaderLabels({"告警ID", "设备ID", "时间戳", "类型", "描述", "级别", "状态"});

    QSqlQuery query("SELECT alarm_id, device_id, timestamp, type, description, level, status FROM alarms ORDER BY timestamp DESC");
    
    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            dataTable->setItem(row, col, item);
        }
        row++;
    }
}

void DatabaseViewer::displaySystemLogs()
{
    dataTable->clear();
    dataTable->setColumnCount(5);
    dataTable->setHorizontalHeaderLabels({"日志ID", "时间戳", "类型", "内容", "用户名"});

    QSqlQuery query("SELECT log_id, timestamp, type, content, username FROM system_logs ORDER BY timestamp DESC LIMIT 100");
    
    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            dataTable->setItem(row, col, item);
        }
        row++;
    }
} 