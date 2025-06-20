#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QStackedWidget>
#include <QToolButton>

// 前向声明
class DatabaseViewer;
class DeviceManagementWindow;
class NetworkMonitorWindow;
class AlarmRuleManagementWindow;
class AlarmDisplayWindow;
class DataAnalysisWindow;
class QButtonGroup;

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
    void onDeviceManagementClicked();
    void onNetworkMonitorClicked();
    void onAlarmRuleManagementClicked();
    void onAlarmDisplayClicked();
    void onDataAnalysisClicked();
    void onLogoutClicked();
    void onExitClicked();
    void smoothSwitchToPage(int pageIndex);

private:
    Ui::AdminWindow *ui;
    DatabaseViewer *databaseViewer;
    DeviceManagementWindow *deviceManagementWindow;
    NetworkMonitorWindow *networkMonitorWindow;
    AlarmRuleManagementWindow *alarmRuleManagementWindow;
    AlarmDisplayWindow *alarmDisplayWindow;
    DataAnalysisWindow *dataAnalysisWindow;
    QString currentUsername;
    QButtonGroup* sideBarGroup;
};

#endif // ADMINWINDOW_H 
