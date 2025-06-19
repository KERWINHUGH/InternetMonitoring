#ifndef NETWORKMONITORWINDOW_H
#define NETWORKMONITORWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>

namespace Ui { class NetworkMonitorWindow; }

class NetworkMonitorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit NetworkMonitorWindow(QWidget *parent = nullptr);
    ~NetworkMonitorWindow();

private slots:
    void onDeviceChanged(int index);
    void onTimeRangeChanged();
    void onExportClicked();
    void refreshRealtimeData();
    void queryHistoryData();

private:
    Ui::NetworkMonitorWindow *ui;
    QTimer *realtimeTimer;
    void setupConnections();
    void updateRealtimeChart();
    void updateHistoryTable();
    void updateHistoryChart();
};

#endif // NETWORKMONITORWINDOW_H 