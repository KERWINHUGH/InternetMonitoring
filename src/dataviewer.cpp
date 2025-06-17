#include "dataviewer.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegExp>

DataViewer::DataViewer(const QString& username, const QString& role, QWidget *parent)
    : QMainWindow(parent), currentUsername(username), userRole(role)
{
    isAdmin = (role == "admin");
    setupUI();
    loadTableData("devices"); // 默认显示设备表
}

DataViewer::~DataViewer()
{
}

void DataViewer::setupUI()
{
    setWindowTitle(isAdmin ? "数据管理" : "数据查看");
    setFixedSize(900, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    controlLayout = new QHBoxLayout();

    // 创建控件
    QLabel *tableLabel = new QLabel("选择表:", this);
    tableComboBox = new QComboBox(this);
    tableComboBox->addItems({"devices", "monitor_data", "alarms", "system_logs"});
    if (isAdmin) {
        tableComboBox->addItem("users");
    }

    refreshButton = new QPushButton("刷新", this);
    exportButton = new QPushButton("导出", this);
    
    // 管理员专用按钮
    addButton = new QPushButton("添加", this);
    editButton = new QPushButton("编辑", this);
    deleteButton = new QPushButton("删除", this);
    
    // 根据权限设置按钮可见性
    addButton->setVisible(isAdmin);
    editButton->setVisible(isAdmin);
    deleteButton->setVisible(isAdmin);
    
    statusLabel = new QLabel("就绪", this);

    // 添加到控制布局
    controlLayout->addWidget(tableLabel);
    controlLayout->addWidget(tableComboBox);
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(exportButton);
    if (isAdmin) {
        controlLayout->addWidget(addButton);
        controlLayout->addWidget(editButton);
        controlLayout->addWidget(deleteButton);
    }
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);

    // 创建数据表格
    dataTable = new QTableWidget(this);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dataTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // 添加到主布局
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(dataTable);

    // 连接信号槽
    connect(tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DataViewer::onTableChanged);
    connect(refreshButton, &QPushButton::clicked, this, &DataViewer::onRefreshClicked);
    connect(exportButton, &QPushButton::clicked, this, &DataViewer::onExportClicked);
    connect(addButton, &QPushButton::clicked, this, &DataViewer::onAddClicked);
    connect(editButton, &QPushButton::clicked, this, &DataViewer::onEditClicked);
    connect(deleteButton, &QPushButton::clicked, this, &DataViewer::onDeleteClicked);
}

void DataViewer::onTableChanged(int index)
{
    QString tableName = tableComboBox->currentText();
    loadTableData(tableName);
}

void DataViewer::onRefreshClicked()
{
    QString tableName = tableComboBox->currentText();
    loadTableData(tableName);
}

void DataViewer::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据", 
                                                   "data_export.csv", 
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

void DataViewer::onAddClicked()
{
    QString tableName = tableComboBox->currentText();
    if (tableName == "users") {
        addUser();
    } else if (tableName == "devices") {
        addDevice();
    }
}

void DataViewer::onEditClicked()
{
    QString tableName = tableComboBox->currentText();
    if (tableName == "users") {
        editUser();
    } else if (tableName == "devices") {
        editDevice();
    }
}

void DataViewer::onDeleteClicked()
{
    QString tableName = tableComboBox->currentText();
    if (tableName == "users") {
        deleteUser();
    } else if (tableName == "devices") {
        deleteDevice();
    }
}

void DataViewer::loadTableData(const QString& tableName)
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

void DataViewer::displayUsers()
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

void DataViewer::displayDevices()
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

void DataViewer::displayMonitorData()
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

void DataViewer::displayAlarms()
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

void DataViewer::displaySystemLogs()
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

void DataViewer::addUser()
{
    bool ok;
    QString username = QInputDialog::getText(this, "添加用户", "用户名:", QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return;

    QString password = QInputDialog::getText(this, "添加用户", "密码:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QString email = QInputDialog::getText(this, "添加用户", "邮箱:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString phone = QInputDialog::getText(this, "添加用户", "手机号:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString nickname = QInputDialog::getText(this, "添加用户", "昵称:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString role = QInputDialog::getItem(this, "添加用户", "角色:", {"user", "admin"}, 0, false, &ok);
    if (!ok) return;

    if (DatabaseManager::instance().addUser(username, password, email, phone, nickname, role)) {
        QMessageBox::information(this, "成功", "用户添加成功");
        displayUsers();
    } else {
        QMessageBox::warning(this, "错误", "用户添加失败");
    }
}

void DataViewer::editUser()
{
    int currentRow = dataTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "错误", "请选择要编辑的用户");
        return;
    }

    QString username = dataTable->item(currentRow, 0)->text();
    QString currentEmail = dataTable->item(currentRow, 2)->text();
    QString currentPhone = dataTable->item(currentRow, 3)->text();
    QString currentNickname = dataTable->item(currentRow, 4)->text();

    bool ok;
    QString email = QInputDialog::getText(this, "编辑用户", "邮箱:", QLineEdit::Normal, currentEmail, &ok);
    if (!ok) return;

    QString phone = QInputDialog::getText(this, "编辑用户", "手机号:", QLineEdit::Normal, currentPhone, &ok);
    if (!ok) return;

    QString nickname = QInputDialog::getText(this, "编辑用户", "昵称:", QLineEdit::Normal, currentNickname, &ok);
    if (!ok) return;

    if (DatabaseManager::instance().updateUser(username, email, phone, nickname)) {
        QMessageBox::information(this, "成功", "用户信息更新成功");
        displayUsers();
    } else {
        QMessageBox::warning(this, "错误", "用户信息更新失败");
    }
}

void DataViewer::deleteUser()
{
    int currentRow = dataTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "错误", "请选择要删除的用户");
        return;
    }

    QString username = dataTable->item(currentRow, 0)->text();
    if (username == "admin") {
        QMessageBox::warning(this, "错误", "不能删除管理员账户");
        return;
    }

    int result = QMessageBox::question(this, "确认删除", 
                                      QString("确定要删除用户 %1 吗？").arg(username),
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteUser(username)) {
            QMessageBox::information(this, "成功", "用户删除成功");
            displayUsers();
        } else {
            QMessageBox::warning(this, "错误", "用户删除失败");
        }
    }
}

void DataViewer::addDevice()
{
    bool ok;
    QString deviceId = QInputDialog::getText(this, "添加设备", "设备ID:", QLineEdit::Normal, "", &ok);
    if (!ok || deviceId.isEmpty()) return;

    QString name = QInputDialog::getText(this, "添加设备", "设备名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString type = QInputDialog::getText(this, "添加设备", "设备类型:", QLineEdit::Normal, "", &ok);
    if (!ok || type.isEmpty()) return;

    QString location = QInputDialog::getText(this, "添加设备", "位置:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    if (DatabaseManager::instance().addDevice(deviceId, name, type, location)) {
        QMessageBox::information(this, "成功", "设备添加成功");
        displayDevices();
    } else {
        QMessageBox::warning(this, "错误", "设备添加失败");
    }
}

void DataViewer::editDevice()
{
    int currentRow = dataTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "错误", "请选择要编辑的设备");
        return;
    }

    QString deviceId = dataTable->item(currentRow, 0)->text();
    QString currentName = dataTable->item(currentRow, 1)->text();
    QString currentType = dataTable->item(currentRow, 2)->text();
    QString currentLocation = dataTable->item(currentRow, 3)->text();

    bool ok;
    QString name = QInputDialog::getText(this, "编辑设备", "设备名称:", QLineEdit::Normal, currentName, &ok);
    if (!ok || name.isEmpty()) return;

    QString type = QInputDialog::getText(this, "编辑设备", "设备类型:", QLineEdit::Normal, currentType, &ok);
    if (!ok || type.isEmpty()) return;

    QString location = QInputDialog::getText(this, "编辑设备", "位置:", QLineEdit::Normal, currentLocation, &ok);
    if (!ok) return;

    if (DatabaseManager::instance().updateDevice(deviceId, name, type, location)) {
        QMessageBox::information(this, "成功", "设备信息更新成功");
        displayDevices();
    } else {
        QMessageBox::warning(this, "错误", "设备信息更新失败");
    }
}

void DataViewer::deleteDevice()
{
    int currentRow = dataTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "错误", "请选择要删除的设备");
        return;
    }

    QString deviceId = dataTable->item(currentRow, 0)->text();
    int result = QMessageBox::question(this, "确认删除", 
                                      QString("确定要删除设备 %1 吗？").arg(deviceId),
                                      QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteDevice(deviceId)) {
            QMessageBox::information(this, "成功", "设备删除成功");
            displayDevices();
        } else {
            QMessageBox::warning(this, "错误", "设备删除失败");
        }
    }
} 