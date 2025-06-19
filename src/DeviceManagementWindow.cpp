#include "DeviceManagementWindow.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

DeviceManagementWindow::DeviceManagementWindow(const QString& currentUsername, bool isAdmin, QWidget *parent)
    : QMainWindow(parent), isAdmin(isAdmin), currentUsername(currentUsername), user_id(-1), ui(new Ui::DeviceManagementWindow)
{
    DatabaseManager::instance().getUserIdByUsername(currentUsername, user_id);
    setupUi();
    setupConnections();
    loadDevices();
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
}

void DeviceManagementWindow::loadDevices()
{
    ui->deviceTable->setRowCount(0);
    auto devices = DatabaseManager::instance().getDevices();
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