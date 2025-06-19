#include "DeviceManagementWindow.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QSet>

DeviceManagementWindow::DeviceManagementWindow(const QString& currentUsername, bool isAdmin, QWidget *parent)
    : QMainWindow(parent), isAdmin(isAdmin), currentUsername(currentUsername), user_id(-1), ui(new Ui::DeviceManagementWindow), currentGroupId(-1)
{
    DatabaseManager::instance().getUserIdByUsername(currentUsername, user_id);
    setupUi();
    setupConnections();
    initGroupTypes();
    if (ui->groupTypeComboBox->count() > 0) {
        ui->groupTypeComboBox->setCurrentIndex(0);
        currentGroupType = ui->groupTypeComboBox->currentText();
        loadGroups();
    }
}

void DeviceManagementWindow::setupUi()
{
    ui->setupUi(this);
}

void DeviceManagementWindow::setupConnections()
{
    connect(ui->addDeviceButton, &QPushButton::clicked, this, &DeviceManagementWindow::onAddDevice);
    connect(ui->editDeviceButton, &QPushButton::clicked, this, &DeviceManagementWindow::onEditDevice);
    connect(ui->deleteDeviceButton, &QPushButton::clicked, this, &DeviceManagementWindow::onDeleteDevice);
    connect(ui->groupTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeviceManagementWindow::onGroupTypeChanged);
    connect(ui->groupComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeviceManagementWindow::onGroupChanged);
    connect(ui->addGroupButton, &QPushButton::clicked, this, &DeviceManagementWindow::onAddGroup);
    connect(ui->renameGroupButton, &QPushButton::clicked, this, &DeviceManagementWindow::onRenameGroup);
    connect(ui->deleteGroupButton, &QPushButton::clicked, this, &DeviceManagementWindow::onDeleteGroup);
}

void DeviceManagementWindow::onGroupTypeChanged(int index)
{
    currentGroupType = ui->groupTypeComboBox->currentText();
    loadGroups();
}

void DeviceManagementWindow::loadGroups()
{
    ui->groupComboBox->blockSignals(true);
    ui->groupComboBox->clear();
    ui->groupComboBox->addItem("未分组", -1);
    auto groups = DatabaseManager::instance().getDeviceGroups(currentGroupType);
    for (const QVariant& v : groups) {
        QVariantMap group = v.toMap();
        ui->groupComboBox->addItem(group["group_name"].toString(), group["group_id"].toInt());
    }
    ui->groupComboBox->blockSignals(false);
    if (ui->groupComboBox->count() > 0) {
        ui->groupComboBox->setCurrentIndex(0);
        onGroupChanged(0);
    } else {
        currentGroupId = -1;
        ui->deviceTable->setRowCount(0);
    }
}

void DeviceManagementWindow::onGroupChanged(int index)
{
    if (index < 0) return;
    currentGroupId = ui->groupComboBox->itemData(index).toInt();
    loadDevices();
}

void DeviceManagementWindow::onAddGroup()
{
    if (currentGroupType.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先选择分组方式！");
        return;
    }
    bool ok = false;
    QString name = QInputDialog::getText(this, "新建分组", "请输入分组名称：", QLineEdit::Normal, "", &ok);
    if (ok && !name.trimmed().isEmpty()) {
        // 校验重名
        auto groups = DatabaseManager::instance().getDeviceGroups(currentGroupType);
        for (const QVariant& v : groups) {
            QVariantMap group = v.toMap();
            if (group["group_name"].toString() == name.trimmed()) {
                QMessageBox::warning(this, "错误", "分组名已存在！");
                return;
            }
        }
        if (DatabaseManager::instance().addDeviceGroup(name.trimmed(), currentGroupType)) {
            loadGroups();
        } else {
            QMessageBox::warning(this, "错误", "新建分组失败！");
        }
    }
}

void DeviceManagementWindow::onRenameGroup()
{
    int idx = ui->groupComboBox->currentIndex();
    if (idx <= 0) return; // 0为未分组
    int groupId = ui->groupComboBox->itemData(idx).toInt();
    QString oldName = ui->groupComboBox->currentText();
    bool ok = false;
    QString newName = QInputDialog::getText(this, "重命名分组", "请输入新分组名称：", QLineEdit::Normal, oldName, &ok);
    if (ok && !newName.trimmed().isEmpty()) {
        // 校验重名
        auto groups = DatabaseManager::instance().getDeviceGroups(currentGroupType);
        for (const QVariant& v : groups) {
            QVariantMap group = v.toMap();
            if (group["group_name"].toString() == newName.trimmed()) {
                QMessageBox::warning(this, "错误", "分组名已存在！");
                return;
            }
        }
        if (DatabaseManager::instance().renameDeviceGroup(groupId, newName.trimmed())) {
            loadGroups();
        } else {
            QMessageBox::warning(this, "错误", "重命名分组失败！");
        }
    }
}

void DeviceManagementWindow::onDeleteGroup()
{
    int idx = ui->groupComboBox->currentIndex();
    if (idx <= 0) return; // 0为未分组
    int groupId = ui->groupComboBox->itemData(idx).toInt();
    QString name = ui->groupComboBox->currentText();
    if (QMessageBox::question(this, "确认删除", QString("确定要删除分组%1吗？该分组下设备不会被删除。\n设备将变为未分组。\n").arg(name)) == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteDeviceGroup(groupId)) {
            loadGroups();
        } else {
            QMessageBox::warning(this, "错误", "删除分组失败！");
        }
    }
}

void DeviceManagementWindow::loadDevices()
{
    ui->deviceTable->setRowCount(0);
    QVariantList devices;
    if (currentGroupId < 0) {
        // 未分组
        devices = DatabaseManager::instance().getDevicesByGroup(-1, true); // true: 查找group_id为NULL
    } else {
        devices = DatabaseManager::instance().getDevicesByGroup(currentGroupId);
    }
    for (const QVariant& v : devices) {
        QVariantMap device = v.toMap();
        int row = ui->deviceTable->rowCount();
        ui->deviceTable->insertRow(row);
        ui->deviceTable->setItem(row, 0, new QTableWidgetItem(QString::number(device["device_id"].toInt())));
        ui->deviceTable->setItem(row, 1, new QTableWidgetItem(device["name"].toString()));
        ui->deviceTable->setItem(row, 2, new QTableWidgetItem(device["type"].toString()));
        ui->deviceTable->setItem(row, 3, new QTableWidgetItem(device["location"].toString()));
        ui->deviceTable->setItem(row, 4, new QTableWidgetItem(device["manufacturer"].toString()));
        ui->deviceTable->setItem(row, 5, new QTableWidgetItem(device["model"].toString()));
        ui->deviceTable->setItem(row, 6, new QTableWidgetItem(device["installation_date"].toString()));
    }
}

int DeviceManagementWindow::getSelectedDeviceId() const
{
    int row = ui->deviceTable->currentRow();
    if (row < 0) return -1;
    return ui->deviceTable->item(row, 0)->text().toInt();
}

void DeviceManagementWindow::onAddDevice()
{
    DeviceEditDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto device = dlg.getDeviceInfo();
        if (DatabaseManager::instance().addDevice(device.name, device.type, device.location,
                                                  device.manufacturer, device.model, device.installation_date)) {
            int device_id;
            if (DatabaseManager::instance().getDeviceIdByName(device.name, device_id)) {
                if (currentGroupId > 0) {
                    DatabaseManager::instance().setDeviceGroup(device_id, currentGroupId);
                } // 未分组不设置group_id
            }
            DatabaseManager::instance().addLog("添加设备", "INFO", QString("添加设备：%1").arg(device.name), user_id);
            loadDevices();
        } else {
            DatabaseManager::instance().addLog("添加设备", "WARN", QString("添加设备失败：%1").arg(device.name), user_id);
            QMessageBox::warning(this, "错误", "添加设备失败！");
        }
    }
}

void DeviceManagementWindow::onEditDevice()
{
    int deviceId = getSelectedDeviceId();
    if (deviceId < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的设备！");
        return;
    }
    QVariantMap device;
    DatabaseManager::instance().getDeviceById(deviceId, device);
    DeviceEditDialog dlg(this);
    dlg.setDeviceInfo(device);
    if (dlg.exec() == QDialog::Accepted) {
        auto dev = dlg.getDeviceInfo();
        if (DatabaseManager::instance().updateDevice(deviceId, dev.name, dev.type, dev.location,
                                                     dev.manufacturer, dev.model, dev.installation_date)) {
            DatabaseManager::instance().addLog("编辑设备", "INFO", QString("编辑设备：%1 (ID:%2)").arg(dev.name).arg(deviceId), user_id, deviceId);
            loadDevices();
        } else {
            DatabaseManager::instance().addLog("编辑设备", "WARN", QString("编辑设备失败：%1 (ID:%2)").arg(dev.name).arg(deviceId), user_id, deviceId);
            QMessageBox::warning(this, "错误", "更新设备失败！");
        }
    }
}

void DeviceManagementWindow::onDeleteDevice()
{
    int deviceId = getSelectedDeviceId();
    if (deviceId < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的设备！");
        return;
    }
    if (QMessageBox::question(this, "确认删除", "确定要删除该设备吗？") == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteDevice(deviceId)) {
            DatabaseManager::instance().addLog("删除设备", "INFO", QString("删除设备 (ID:%1)").arg(deviceId), user_id, deviceId);
            loadDevices();
        } else {
            DatabaseManager::instance().addLog("删除设备", "WARN", QString("删除设备失败 (ID:%1)").arg(deviceId), user_id, deviceId);
            QMessageBox::warning(this, "错误", "删除设备失败！");
        }
    }
}

void DeviceManagementWindow::initGroupTypes() {
    ui->groupTypeComboBox->clear();
    QSet<QString> groupTypes;
    auto allGroups = DatabaseManager::instance().getAllDeviceGroups();
    for (const QVariant& v : allGroups) {
        QVariantMap group = v.toMap();
        groupTypes.insert(group["group_type"].toString());
    }
    for (const QString& type : groupTypes) {
        ui->groupTypeComboBox->addItem(type);
    }
    // 如无分组，默认添加"类型""位置"
    if (ui->groupTypeComboBox->count() == 0) {
        ui->groupTypeComboBox->addItem("类型");
        ui->groupTypeComboBox->addItem("位置");
    }
}
