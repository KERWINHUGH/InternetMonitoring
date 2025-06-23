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

DatabaseViewer::DatabaseViewer(QWidget *parent, const QStringList& tables, bool readonly)
    : QWidget(parent), customTables(tables), m_readonly(readonly)
{
    setupUI();
    if (!customTables.isEmpty()) {
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
    setMinimumSize(800, 600);

    mainLayout = new QVBoxLayout(this);
    controlLayout = new QHBoxLayout();

    tableLabel = new QLabel("选择表:", this);
    tableComboBox = new QComboBox(this);
    QSqlDatabase db = QSqlDatabase::database();
    QStringList tables = db.tables();
    tables.removeDuplicates();
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
    saveButton = new QPushButton("修改", this);
    statusLabel = new QLabel("就绪", this);

    controlLayout->addWidget(tableLabel);
    controlLayout->addWidget(tableComboBox);
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(exportButton);
    if (!m_readonly) {
        controlLayout->addWidget(addButton);
        controlLayout->addWidget(deleteButton);
        controlLayout->addWidget(saveButton);
    } else {
        addButton->hide();
        deleteButton->hide();
        saveButton->hide();
    }
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);

    dataTable = new QTableWidget(this);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    if (m_readonly) {
        dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(dataTable);

    connect(tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DatabaseViewer::onTableChanged);
    connect(refreshButton, &QPushButton::clicked, this, &DatabaseViewer::onRefreshClicked);
    connect(exportButton, &QPushButton::clicked, this, &DatabaseViewer::onExportClicked);
    if (!m_readonly) {
        qDebug() << "addButton address:" << addButton;
        connect(addButton, &QPushButton::clicked, this, &DatabaseViewer::onAddClicked);
        connect(deleteButton, &QPushButton::clicked, this, &DatabaseViewer::onDeleteClicked);
        connect(saveButton, &QPushButton::clicked, this, &DatabaseViewer::onEditClicked);
        connect(dataTable, &QTableWidget::cellChanged, this, &DatabaseViewer::onCellChanged);
        connect(dataTable, &QTableWidget::cellDoubleClicked, this, &DatabaseViewer::onCellDoubleClicked);
        connect(addButton, &QPushButton::clicked, [](){ qDebug() << "addButton clicked!"; });
    }
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
    qDebug() << "onAddClicked called";
    UserEditDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto info = dlg.getUserInfo();
        if (info.username.isEmpty() || info.email.isEmpty() || info.phone.isEmpty()) {
            QMessageBox::warning(this, "提示", "用户名、邮箱和手机号不能为空！");
            return;
        }
        if (QMessageBox::question(this, "确认添加", QString("确定要添加用户 %1 吗？").arg(info.username),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            return;
        }
        if (DatabaseManager::instance().addUser(info.username, info.password.isEmpty() ? "123456" : info.password,
                                                info.email, info.phone, info.nickname, info.role)) {
            loadTableData("users");
            QMessageBox::information(this, "成功", "用户添加成功！");
        } else {
            QMessageBox::warning(this, "失败", "添加用户失败：" + DatabaseManager::instance().lastError());
        }
    }
}

void DatabaseViewer::onDeleteClicked()
{
    if (tableComboBox->currentText() != "users") return;
    int row = dataTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请先选中要删除的用户行。");
        return;
    }
    QTableWidgetItem* idItem = dataTable->item(row, 0);
    if (!idItem) {
        QMessageBox::warning(this, "警告", "无法获取用户ID。");
        return;
    }
    int userId = idItem->data(Qt::UserRole).toInt();
    qDebug() << "delete userId:" << userId;
    QString username = dataTable->item(row, 1) ? dataTable->item(row, 1)->text() : "";
    if (userId <= 0) {
        QMessageBox::warning(this, "警告", "只能删除已保存到数据库的用户。");
        return;
    }
    if (QMessageBox::question(this, "确认删除", QString("确定要删除用户 %1 吗？此操作不可恢复。\n").arg(username),
                              QMessageBox::Yes|QMessageBox::No) == QMessageBox::No) {
        return;
    }
    if (DatabaseManager::instance().deleteUser(userId)) {
        loadTableData("users");
        QMessageBox::information(this, "成功", "用户已删除！");
    } else {
        QMessageBox::critical(this, "删除失败", "无法从数据库删除用户 " + QString::number(userId) + "\n" + DatabaseManager::instance().lastError());
    }
}

void DatabaseViewer::onEditClicked()
{
    if (tableComboBox->currentText() != "users") return;
    int row = dataTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请先选中要修改的用户行。");
        return;
    }
    int userId = dataTable->item(row, 0)->data(Qt::UserRole).toInt();
    qDebug() << "edit userId:" << userId;
    UserEditDialog dlg(this);
    dlg.setUserInfo(
        dataTable->item(row, 1) ? dataTable->item(row, 1)->text() : "",
        dataTable->item(row, 3) ? dataTable->item(row, 3)->text() : "",
        dataTable->item(row, 4) ? dataTable->item(row, 4)->text() : "",
        dataTable->item(row, 5) ? dataTable->item(row, 5)->text() : "",
        dataTable->item(row, 6) ? dataTable->item(row, 6)->text() : "user"
    );
    if (dlg.exec() == QDialog::Accepted) {
        auto info = dlg.getUserInfo();
        if (info.email.isEmpty() || info.phone.isEmpty()) {
            QMessageBox::warning(this, "提示", "邮箱和手机号不能为空！");
            return;
        }
        if (QMessageBox::question(this, "确认修改", QString("确定要修改用户 %1 的信息吗？").arg(info.username),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            return;
        }
        if (DatabaseManager::instance().updateUser(userId, info.email, info.phone, info.nickname)) {
            loadTableData("users");
            QMessageBox::information(this, "成功", "用户信息已更新！");
        } else {
            QMessageBox::warning(this, "失败", "更新失败：" + DatabaseManager::instance().lastError());
        }
    }
}

void DatabaseViewer::onCellChanged(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    // 空实现
}

void DatabaseViewer::onCellDoubleClicked(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (tableComboBox->currentText() != "users") return;
    int userId = dataTable->item(row, 0)->data(Qt::UserRole).toInt();
    UserEditDialog dlg(this);
    dlg.setUserInfo(
        dataTable->item(row, 1) ? dataTable->item(row, 1)->text() : "",
        dataTable->item(row, 3) ? dataTable->item(row, 3)->text() : "",
        dataTable->item(row, 4) ? dataTable->item(row, 4)->text() : "",
        dataTable->item(row, 5) ? dataTable->item(row, 5)->text() : "",
        dataTable->item(row, 6) ? dataTable->item(row, 6)->text() : "user"
    );
    if (dlg.exec() == QDialog::Accepted) {
        auto info = dlg.getUserInfo();
        if (info.email.isEmpty() || info.phone.isEmpty()) {
            QMessageBox::warning(this, "提示", "邮箱和手机号不能为空！");
            return;
        }
        if (QMessageBox::question(this, "确认修改", QString("确定要修改用户 %1 的信息吗？").arg(info.username),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            return;
        }
        if (DatabaseManager::instance().updateUser(userId, info.email, info.phone, info.nickname)) {
            loadTableData("users");
            QMessageBox::information(this, "成功", "用户信息已更新！");
        } else {
            QMessageBox::warning(this, "失败", "更新失败：" + DatabaseManager::instance().lastError());
        }
    }
}

void DatabaseViewer::onSaveChangesClicked() {} 
