#ifndef DEVICEMANAGEMENTWINDOW_H
#define DEVICEMANAGEMENTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include "DeviceEditDialog.h"

class DeviceManagementWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DeviceManagementWindow(const QString& currentUsername, bool isAdmin, QWidget *parent = nullptr);

private slots:
    void loadDevices();
    void onAddDevice();
    void onEditDevice();
    void onDeleteDevice();

private:
    QTableWidget* deviceTable;
    QPushButton* addDeviceButton;
    QPushButton* editDeviceButton;
    QPushButton* deleteDeviceButton;
    bool isAdmin;
    QString currentUsername;
    int user_id;
    void setupUi();
    void setupConnections();
    int getSelectedDeviceId() const;
};

#endif // DEVICEMANAGEMENTWINDOW_H 