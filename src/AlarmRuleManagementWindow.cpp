#include "AlarmRuleManagementWindow.h"
#include "ui_AlarmRuleManagementWindow.h"
#include "databasemanager.h"
#include "alarmruleeditdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>

AlarmRuleManagementWindow::AlarmRuleManagementWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AlarmRuleManagementWindow)
{
    ui->setupUi(this);

    connect(ui->addRuleButton, &QPushButton::clicked, this, &AlarmRuleManagementWindow::onAddRule);
    connect(ui->editRuleButton, &QPushButton::clicked, this, &AlarmRuleManagementWindow::onEditRule);
    connect(ui->deleteRuleButton, &QPushButton::clicked, this, &AlarmRuleManagementWindow::onDeleteRule);

    loadRules();
}

AlarmRuleManagementWindow::~AlarmRuleManagementWindow()
{
    delete ui;
}

void AlarmRuleManagementWindow::onAddRule()
{
    AlarmRuleEditDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QVariantMap ruleData = dlg.getRuleData();
        if (ruleData["description"].toString().isEmpty() || ruleData["condition"].toString().isEmpty()) {
            QMessageBox::warning(this, "警告", "规则描述和触发条件不能为空。");
            return;
        }

        if (DatabaseManager::instance().addAlarmRule(ruleData["device_id"].toInt(),
                                                    ruleData["description"].toString(),
                                                    ruleData["condition"].toString(),
                                                    ruleData["action"].toString())) {
            DatabaseManager::instance().addLog("告警规则", "INFO", QString("添加规则：%1").arg(ruleData["description"].toString()), -1, ruleData["device_id"].toInt());
            QMessageBox::information(this, "成功", "新告警规则已添加。");
            loadRules();
        } else {
            QMessageBox::critical(this, "添加失败", "无法添加告警规则。\n" + DatabaseManager::instance().lastError());
        }
    }
}

void AlarmRuleManagementWindow::onEditRule()
{
    int currentRow = ui->ruleTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要编辑的规则。");
        return;
    }

    QVariantMap ruleData;
    ruleData["rule_id"] = ui->ruleTable->item(currentRow, 0)->text().toInt();
    ruleData["device_id"] = ui->ruleTable->item(currentRow, 1)->text().toInt();
    ruleData["description"] = ui->ruleTable->item(currentRow, 3)->text();
    ruleData["condition"] = ui->ruleTable->item(currentRow, 4)->text();
    ruleData["action"] = ui->ruleTable->item(currentRow, 5)->text();

    AlarmRuleEditDialog dlg(this);
    dlg.setRuleData(ruleData);

    if (dlg.exec() == QDialog::Accepted) {
        QVariantMap newRuleData = dlg.getRuleData();
        if (newRuleData["description"].toString().isEmpty() || newRuleData["condition"].toString().isEmpty()) {
            QMessageBox::warning(this, "警告", "规则描述和触发条件不能为空。");
            return;
        }

        if (DatabaseManager::instance().updateAlarmRule(ruleData["rule_id"].toInt(),
                                                        newRuleData["device_id"].toInt(),
                                                        newRuleData["description"].toString(),
                                                        newRuleData["condition"].toString(),
                                                        newRuleData["action"].toString())) {
            DatabaseManager::instance().addLog("告警规则", "INFO", QString("编辑规则：%1").arg(newRuleData["description"].toString()), -1, newRuleData["device_id"].toInt());
            QMessageBox::information(this, "成功", "告警规则已更新。");
            loadRules();
        } else {
            QMessageBox::critical(this, "更新失败", "无法更新告警规则。\n" + DatabaseManager::instance().lastError());
        }
    }
}

void AlarmRuleManagementWindow::onDeleteRule()
{
    int currentRow = ui->ruleTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要删除的规则。");
        return;
    }

    int ruleId = ui->ruleTable->item(currentRow, 0)->text().toInt();
    QString ruleDesc = ui->ruleTable->item(currentRow, 3)->text();

    int ret = QMessageBox::question(this, "确认删除",
                                    QString("您确定要删除规则%1吗？\n此操作不可恢复.").arg(ruleDesc),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteAlarmRule(ruleId)) {
            DatabaseManager::instance().addLog("告警规则", "INFO", QString("删除规则：%1").arg(ruleDesc), -1);
            QMessageBox::information(this, "成功", "告警规则已删除。");
            loadRules(); // 重新加载规则列表
        } else {
            QMessageBox::critical(this, "删除失败", "无法删除告警规则。\n" + DatabaseManager::instance().lastError());
        }
    }
}

void AlarmRuleManagementWindow::loadRules()
{
    ui->ruleTable->clearContents();
    ui->ruleTable->setRowCount(0);
    ui->ruleTable->setColumnCount(6); // rule_id, device_id, device_name, description, condition, action
    ui->ruleTable->setHorizontalHeaderLabels({"规则ID", "设备ID", "设备名称", "描述", "条件", "动作"});
    ui->ruleTable->setColumnHidden(0, true); // 隐藏规则ID
    ui->ruleTable->setColumnHidden(1, true); // 隐藏设备ID

    ui->ruleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ruleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ruleTable->horizontalHeader()->setStretchLastSection(true);

    QVariantList rules = DatabaseManager::instance().getAlarmRules(-1); // -1 获取所有规则

    int row = 0;
    for (const QVariant& ruleVariant : rules) {
        QVariantMap rule = ruleVariant.toMap();
        ui->ruleTable->insertRow(row);

        // 获取设备名称
        QVariantMap device;
        QString deviceName = "未知设备";
        if (DatabaseManager::instance().getDeviceById(rule["device_id"].toInt(), device)) {
            deviceName = device["name"].toString();
        }

        QTableWidgetItem* idItem = new QTableWidgetItem(rule["rule_id"].toString());
        QTableWidgetItem* deviceIdItem = new QTableWidgetItem(rule["device_id"].toString());
        QTableWidgetItem* deviceNameItem = new QTableWidgetItem(deviceName);
        QTableWidgetItem* descriptionItem = new QTableWidgetItem(rule["description"].toString());
        QTableWidgetItem* conditionItem = new QTableWidgetItem(rule["condition"].toString());
        QTableWidgetItem* actionItem = new QTableWidgetItem(rule["action"].toString());

        ui->ruleTable->setItem(row, 0, idItem);
        ui->ruleTable->setItem(row, 1, deviceIdItem);
        ui->ruleTable->setItem(row, 2, deviceNameItem);
        ui->ruleTable->setItem(row, 3, descriptionItem);
        ui->ruleTable->setItem(row, 4, conditionItem);
        ui->ruleTable->setItem(row, 5, actionItem);

        row++;
    }
} 
