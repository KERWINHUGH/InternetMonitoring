#include "alarmruleeditdialog.h"
#include "databasemanager.h"
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

AlarmRuleEditDialog::AlarmRuleEditDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    loadDevices();
    setWindowTitle("编辑告警规则");
}

void AlarmRuleEditDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    deviceComboBox = new QComboBox(this);
    descriptionLineEdit = new QLineEdit(this);
    conditionTextEdit = new QTextEdit(this);
    actionTextEdit = new QTextEdit(this);

    conditionTextEdit->setPlaceholderText("例如: temperature > 30 AND humidity < 50");
    actionTextEdit->setPlaceholderText("例如: SEND_EMAIL foo@bar.com");

    formLayout->addRow("关联设备:", deviceComboBox);
    formLayout->addRow("规则描述:", descriptionLineEdit);
    formLayout->addRow("触发条件:", conditionTextEdit);
    formLayout->addRow("执行动作:", actionTextEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
}

void AlarmRuleEditDialog::loadDevices()
{
    deviceComboBox->clear();
    QVariantList devices = DatabaseManager::instance().getDevices();
    for (const QVariant& deviceVariant : devices) {
        QVariantMap device = deviceVariant.toMap();
        deviceComboBox->addItem(device["name"].toString(), device["device_id"]);
    }
}

void AlarmRuleEditDialog::setRuleData(const QVariantMap &ruleData)
{
    // Find and set the device in the combo box
    int deviceId = ruleData.value("device_id").toInt();
    int index = deviceComboBox->findData(deviceId);
    if (index != -1) {
       deviceComboBox->setCurrentIndex(index);
    }

    descriptionLineEdit->setText(ruleData.value("description").toString());
    conditionTextEdit->setText(ruleData.value("condition").toString());
    actionTextEdit->setText(ruleData.value("action").toString());
}

QVariantMap AlarmRuleEditDialog::getRuleData() const
{
    QVariantMap ruleData;
    ruleData["device_id"] = deviceComboBox->currentData().toInt();
    ruleData["description"] = descriptionLineEdit->text();
    ruleData["condition"] = conditionTextEdit->toPlainText();
    ruleData["action"] = actionTextEdit->toPlainText();
    return ruleData;
} 