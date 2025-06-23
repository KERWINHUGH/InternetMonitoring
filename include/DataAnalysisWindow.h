#ifndef DATAANALYSISWINDOW_H
#define DATAANALYSISWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

QT_CHARTS_USE_NAMESPACE

namespace Ui { class DataAnalysisWindow; }

class DataAnalysisWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DataAnalysisWindow(QWidget *parent = nullptr);
    ~DataAnalysisWindow();

private slots:
    void onAnalysisClicked();
    void onExportClicked();

private:
    void setupUiElements();
    void setupChart();
    void loadDeviceList();
    void performAnalysis(int deviceId, const QString& dataType, const QDateTime& startTime, const QDateTime& endTime);
    void updateTable(const QList<QVariantMap>& analysisResult);
    void updateChart(const QList<QVariantMap>& analysisResult);

    Ui::DataAnalysisWindow *ui;
    QChartView* chartView;
    QChart* chart;
    QBarSeries* series;
};

#endif // DATAANALYSISWINDOW_H 