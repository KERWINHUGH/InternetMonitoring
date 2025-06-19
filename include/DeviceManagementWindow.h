#ifndef DEVICEMANAGEMENTWINDOW_H
#define DEVICEMANAGEMENTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include "DeviceEditDialog.h"
#include "ui_DeviceManagementWindow.h"

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
    void onAddGroup();
    void onRenameGroup();
    void onDeleteGroup();
    void onGroupTypeChanged(int index);
    void onGroupChanged(int index);

private:
    Ui::DeviceManagementWindow *ui;
    bool isAdmin;
    QString currentUsername;
    int user_id;
    QString currentGroupType;
    int currentGroupId;
    void setupUi();
    void setupConnections();
    int getSelectedDeviceId() const;
    void loadGroups();
    void initGroupTypes();
};

#endif // DEVICEMANAGEMENTWINDOW_H 
