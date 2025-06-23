#ifndef NETWORKMONITORWINDOW_H
#define NETWORKMONITORWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

namespace Ui { class NetworkMonitorWindow; }

class NetworkMonitorWindow : public QWidget
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
    
    // 实时图表
    QChartView *realtimeChartView;
    QChart *realtimeChart;
    QLineSeries *tempSeriesRealtime;
    QLineSeries *humiditySeriesRealtime;
    QLineSeries *lightSeriesRealtime;

    // 历史图表
    QChartView *historyChartView;
    QChart *historyChart;
    QLineSeries *tempSeriesHistory;
    QLineSeries *humiditySeriesHistory;
    QLineSeries *lightSeriesHistory;

    void setupUiElements();
    void setupCharts();
    void loadDeviceList();
    void updateRealtimeChart(const QVariantMap &data);
    void updateHistoryUi(const QVariantList &data);
};

#endif // NETWORKMONITORWINDOW_H 