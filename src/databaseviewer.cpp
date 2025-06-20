#include "databaseviewer.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlDatabase>

//DatabaseViewer::DatabaseViewer(QWidget *parent)
//    : QMainWindow(parent)
//{
//    setupUI();
//    loadTableData("users"); // 默认显示用户表
//}

DatabaseViewer::~DatabaseViewer()
{
}

DatabaseViewer::DatabaseViewer(QWidget *parent, const QStringList& tables)
    : QMainWindow(parent), customTables(tables)
{
    setupUI();
    if (!customTables.isEmpty()) {
        // 如果只提供一个表，则隐藏选择框
        if (customTables.count() == 1) {
            tableLabel->hide();
            tableComboBox->hide();
        }
        loadTableData(customTables.first());
    } else {
        loadTableData("users");
    }
}

void DatabaseViewer::setupUI()
{
    setWindowTitle("数据库查看器");
    // setFixedSize(800, 600); // 移除固定大小，使其可缩放
    setMinimumSize(800, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    controlLayout = new QHBoxLayout();

    // 创建控件
    tableLabel = new QLabel("选择表:", this);

    tableComboBox = new QComboBox(this);
    QSqlDatabase db = QSqlDatabase::database();
    QStringList tables = db.tables();
    tables.removeDuplicates(); // 去重
    for (int i = tables.size() - 1; i >= 0; --i) {
        if (tables[i].startsWith("sqlite_", Qt::CaseInsensitive)) {
            tables.removeAt(i);
        }
    }
    tableComboBox->addItems(tables);

    refreshButton = new QPushButton("刷新", this);
    exportButton = new QPushButton("导出", this);
    addButton = new QPushButton("添加", this);
    deleteButton = new QPushButton("删除", this);
    saveButton = new QPushButton("保存更改", this);
    statusLabel = new QLabel("就绪", this);

    // 添加到控制布局
    controlLayout->addWidget(tableLabel);
    controlLayout->addWidget(tableComboBox);
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(exportButton);
    controlLayout->addWidget(addButton);
    controlLayout->addWidget(deleteButton);
    controlLayout->addWidget(saveButton);
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);

    // 创建数据表格
    dataTable = new QTableWidget(this);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 添加到主布局
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(dataTable);

    // 连接信号槽
    connect(tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DatabaseViewer::onTableChanged);
    connect(refreshButton, &QPushButton::clicked, this, &DatabaseViewer::onRefreshClicked);
    connect(exportButton, &QPushButton::clicked, this, &DatabaseViewer::onExportClicked);
    connect(addButton, &QPushButton::clicked, this, &DatabaseViewer::onAddClicked);
    connect(deleteButton, &QPushButton::clicked, this, &DatabaseViewer::onDeleteClicked);
    connect(saveButton, &QPushButton::clicked, this, &DatabaseViewer::onSaveChangesClicked);
    connect(dataTable, &QTableWidget::cellChanged, this, &DatabaseViewer::onCellChanged);
}

void DatabaseViewer::onTableChanged()
{
    QString tableName = tableComboBox->currentText();
    loadTableData(tableName);
}

void DatabaseViewer::onRefreshClicked()
{
    QString tableName;
    if (tableComboBox->isVisible()) {
        tableName = tableComboBox->currentText();
    } else if (!customTables.isEmpty()) {
        // 如果下拉框被隐藏，使用自定义表列表中的第一个表
        tableName = customTables.first();
    } else {
        tableName = "users"; // 默认值
    }
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
    disconnect(dataTable, &QTableWidget::cellChanged, this, &DatabaseViewer::onCellChanged);
    
    statusLabel->setText("正在加载数据...");
    changedRows.clear();

    bool showEditButtons = (tableName == "users");
    addButton->setVisible(showEditButtons);
    deleteButton->setVisible(showEditButtons);
    saveButton->setVisible(showEditButtons);

    if (tableName == "users") {
        displayUsers();
    } else if (tableName == "devices") {
        displayDevices();
    } else if (tableName == "monitor_data") {
        displayMonitorData();
    } else if (tableName == "alarm_rules") {
        displayAlarmRules();
    } else if (tableName == "alarm_records") {
        displayAlarmRecords();
    } else if (tableName == "system_logs") {
        displaySystemLogs();
    } else if (tableName == "device_groups") {
        displayDeviceGroups();
    }

    connect(dataTable, &QTableWidget::cellChanged, this, &DatabaseViewer::onCellChanged);

    statusLabel->setText(QString("已加载 %1 条记录").arg(dataTable->rowCount()));
}

void DatabaseViewer::displayUsers()
{
    dataTable->clear();
    dataTable->setColumnCount(7);
    dataTable->setHorizontalHeaderLabels({"用户ID", "用户名", "密码", "邮箱", "手机号", "昵称", "角色"});
    dataTable->setColumnHidden(0, true);
    dataTable->setColumnHidden(2, true);

    QSqlQuery query("SELECT user_id, username, password, email, phone, nickname, role FROM users");
    
    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            if (col == 0 || col == 6) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
            dataTable->setItem(row, col, item);
        }
        dataTable->item(row, 0)->setData(Qt::UserRole, query.value(0));
        row++;
    }
}

void DatabaseViewer::displayDeviceGroups()
{
    dataTable->clear();
    dataTable->setColumnCount(3);
    dataTable->setHorizontalHeaderLabels({"分组ID", "分组名", "分组类型"});

    QSqlQuery query("SELECT group_id, group_name, group_type FROM device_groups");

    int row = 0;
    while (query.next()) {
        dataTable->insertRow(row);
        for (int col = 0; col < 3; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            dataTable->setItem(row, col, item);
        }
        row++;
    }
}

void DatabaseViewer::displayDevices()
{
    dataTable->clear();
    dataTable->setColumnCount(7);
    dataTable->setHorizontalHeaderLabels({"设备ID", "名称", "类型", "位置", "制造商", "型号", "安装日期"});

    QSqlQuery query("SELECT device_id, name, type, location, manufacturer, model, installation_date FROM devices");
    
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

void DatabaseViewer::displayMonitorData()
{
    dataTable->clear();
    dataTable->setColumnCount(6);
    dataTable->setHorizontalHeaderLabels({"数据ID", "设备ID", "时间戳", "温度", "湿度", "光照"});

    QSqlQuery query("SELECT data_id, device_id, timestamp, temperature, humidity, light FROM monitor_data ORDER BY timestamp DESC LIMIT 100");
    
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

void DatabaseViewer::displayAlarmRules()
{
    dataTable->clear();
    dataTable->setColumnCount(5);
    dataTable->setHorizontalHeaderLabels({"规则ID", "设备ID", "描述", "条件", "动作"});

    QSqlQuery query("SELECT rule_id, device_id, description, condition, action FROM alarm_rules");
    
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

void DatabaseViewer::displayAlarmRecords()
{
    dataTable->clear();
    dataTable->setColumnCount(6);
    dataTable->setHorizontalHeaderLabels({"告警ID", "设备ID", "时间戳", "内容", "状态", "备注"});

    QSqlQuery query("SELECT alarm_id, device_id, timestamp, content, status, note FROM alarm_records ORDER BY timestamp DESC");
    
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

void DatabaseViewer::displaySystemLogs()
{
    dataTable->clear();
    dataTable->setColumnCount(7);
    dataTable->setHorizontalHeaderLabels({"日志ID", "时间戳", "类型", "级别", "内容", "用户ID", "设备ID"});

    QSqlQuery query("SELECT log_id, timestamp, log_type, log_level, content, user_id, device_id FROM system_logs ORDER BY timestamp DESC LIMIT 100");
    
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

void DatabaseViewer::onAddClicked()
{
    if (tableComboBox->currentText() != "users") return;

    int newRow = dataTable->rowCount();
    dataTable->insertRow(newRow);

    // 为新行添加默认值
    dataTable->setItem(newRow, 1, new QTableWidgetItem("new_user")); // username
    // 密码在保存时处理
    dataTable->setItem(newRow, 3, new QTableWidgetItem("user@example.com")); // email
    dataTable->setItem(newRow, 4, new QTableWidgetItem("12345678901")); // phone
    dataTable->setItem(newRow, 5, new QTableWidgetItem("新用户")); // nickname
    QTableWidgetItem* roleItem = new QTableWidgetItem("user");
    roleItem->setFlags(roleItem->flags() & ~Qt::ItemIsEditable); // 角色默认为user且不可编辑
    dataTable->setItem(newRow, 6, roleItem); // role

    // 将新行标记为已更改
    changedRows[newRow]["is_new"] = true;
    statusLabel->setText("添加了一行新数据，请编辑后保存。");
}

void DatabaseViewer::onDeleteClicked()
{
    if (tableComboBox->currentText() != "users") return;

    QList<QTableWidgetItem*> selectedItems = dataTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的行。");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "确定要删除选中的用户吗？此操作不可恢复。",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    // 从UI和数据库中删除
    // 使用QSet来处理唯一的行
    QSet<int> rowsToDelete;
    for(auto item : selectedItems) {
        rowsToDelete.insert(item->row());
    }

    // 从后往前删除，避免行号错乱
    QList<int> sortedRows = rowsToDelete.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

    int successCount = 0;
    for(int row : sortedRows) {
        QTableWidgetItem* idItem = dataTable->item(row, 0);
        // 如果是尚未保存的新行，直接从UI移除
        if(changedRows.contains(row) && changedRows[row].value("is_new").toBool()) {
             dataTable->removeRow(row);
             changedRows.remove(row);
             successCount++;
             continue;
        }

        // 否则从数据库删除
        if(idItem) {
            int userId = idItem->data(Qt::UserRole).toInt();
            if (DatabaseManager::instance().deleteUser(userId)) {
                dataTable->removeRow(row);
                successCount++;
            } else {
                QMessageBox::critical(this, "删除失败", "无法从数据库删除用户 " + QString::number(userId));
            }
        }
    }
    statusLabel->setText(QString("成功删除了 %1 条记录。").arg(successCount));
}

void DatabaseViewer::onSaveChangesClicked()
{
    if (tableComboBox->currentText() != "users") return;
    if (changedRows.isEmpty()) {
        QMessageBox::information(this, "提示", "没有任何更改需要保存。");
        return;
    }

    int updatedCount = 0;
    int addedCount = 0;

    DatabaseManager& dbManager = DatabaseManager::instance();

    for (auto it = changedRows.begin(); it != changedRows.end(); ++it) {
        int row = it.key();
        QVariantMap changes = it.value();

        if (changes.value("is_new").toBool()) {
            // 添加新用户
            QString username = dataTable->item(row, 1)->text();
            QString password = "123456"; // 新用户的默认密码
            QString email = dataTable->item(row, 3)->text();
            QString phone = dataTable->item(row, 4)->text();
            QString nickname = dataTable->item(row, 5)->text();
            QString role = dataTable->item(row, 6)->text();
            if (dbManager.addUser(username, password, email, phone, nickname, role)) {
                addedCount++;
            } else {
                 QMessageBox::warning(this, "添加失败", "无法添加用户: " + username + "\n错误: " + dbManager.lastError());
            }
        } else {
            // 更新现有用户
            int userId = dataTable->item(row, 0)->data(Qt::UserRole).toInt();
            QString email = changes.contains("email") ? changes["email"].toString() : dataTable->item(row, 3)->text();
            QString phone = changes.contains("phone") ? changes["phone"].toString() : dataTable->item(row, 4)->text();
            QString nickname = changes.contains("nickname") ? changes["nickname"].toString() : dataTable->item(row, 5)->text();

            if (dbManager.updateUser(userId, email, phone, nickname)) {
                updatedCount++;
            } else {
                QMessageBox::warning(this, "更新失败", "无法更新用户ID: " + QString::number(userId));
            }
        }
    }

    QMessageBox::information(this, "保存成功", QString("成功添加 %1 个新用户，更新 %2 个用户信息。").arg(addedCount).arg(updatedCount));
    loadTableData("users"); // 重新加载数据
}

void DatabaseViewer::onCellChanged(int row, int column)
{
    // 忽略ID和密码列的更改
    if (column == 0 || column == 2) return;

    // 如果是新行，已经在onAddClicked中标记
    if(changedRows.contains(row) && changedRows[row].value("is_new").toBool()) {
        return;
    }

    // 获取表头和当前单元格内容
    QString fieldName = dataTable->horizontalHeaderItem(column)->text();
    QTableWidgetItem *item = dataTable->item(row, column);
    if (!item) return;

    // 记录更改
    changedRows[row][fieldName] = item->text();
    saveButton->setStyleSheet("background-color: #f44336; color: white;"); // 高亮保存按钮
    statusLabel->setText("有未保存的更改。");
} 
