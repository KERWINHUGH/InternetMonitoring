#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

// 前向声明
class DatabaseViewer;
class DeviceManagementWindow;

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(const QString& currentUsername, QWidget *parent = nullptr);
    ~AdminWindow() override;

signals:
    void windowClosed();  // 窗口关闭信号

private slots:
    void onUserManagementClicked();
    void onSystemSettingsClicked();
    void onDeviceManagementClicked();
    void onLogoutClicked();
    void onCloseClicked();

private:
    Ui::AdminWindow *ui;
    DatabaseViewer *databaseViewer;
    DeviceManagementWindow *deviceManagementWindow;
    QString currentUsername;
};

#endif // ADMINWINDOW_H 
