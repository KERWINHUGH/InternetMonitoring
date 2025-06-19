#ifndef DEVICEEDITDIALOG_H
#define DEVICEEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QVariantMap>

struct DeviceInfo {
    QString name, type, location, manufacturer, model, installation_date;
};

class DeviceEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceEditDialog(QWidget *parent = nullptr);
    void setDeviceInfo(const QVariantMap& device);
    DeviceInfo getDeviceInfo() const;

private:
    QLineEdit* nameEdit;
    QLineEdit* typeEdit;
    QLineEdit* locationEdit;
    QLineEdit* manufacturerEdit;
    QLineEdit* modelEdit;
    QDateEdit* installDateEdit;
};

#endif // DEVICEEDITDIALOG_H 