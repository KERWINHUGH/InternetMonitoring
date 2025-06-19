#include "DeviceEditDialog.h"
#include <QFormLayout>
#include <QPushButton>
#include <QHBoxLayout>

DeviceEditDialog::DeviceEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("设备信息");
    QFormLayout* form = new QFormLayout(this);
    nameEdit = new QLineEdit(this);
    typeEdit = new QLineEdit(this);
    locationEdit = new QLineEdit(this);
    manufacturerEdit = new QLineEdit(this);
    modelEdit = new QLineEdit(this);
    installDateEdit = new QDateEdit(this);
    installDateEdit->setCalendarPopup(true);
    form->addRow("名称：", nameEdit);
    form->addRow("类型：", typeEdit);
    form->addRow("位置：", locationEdit);
    form->addRow("制造商：", manufacturerEdit);
    form->addRow("型号：", modelEdit);
    form->addRow("安装日期：", installDateEdit);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* okBtn = new QPushButton("确定", this);
    QPushButton* cancelBtn = new QPushButton("取消", this);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    form->addRow(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void DeviceEditDialog::setDeviceInfo(const QVariantMap& device)
{
    nameEdit->setText(device["name"].toString());
    typeEdit->setText(device["type"].toString());
    locationEdit->setText(device["location"].toString());
    manufacturerEdit->setText(device["manufacturer"].toString());
    modelEdit->setText(device["model"].toString());
    installDateEdit->setDate(QDate::fromString(device["installation_date"].toString(), "yyyy-MM-dd"));
}

DeviceInfo DeviceEditDialog::getDeviceInfo() const
{
    DeviceInfo info;
    info.name = nameEdit->text();
    info.type = typeEdit->text();
    info.location = locationEdit->text();
    info.manufacturer = manufacturerEdit->text();
    info.model = modelEdit->text();
    info.installation_date = installDateEdit->date().toString("yyyy-MM-dd");
    return info;
} 