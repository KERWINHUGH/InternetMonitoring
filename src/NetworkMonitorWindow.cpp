#include "NetworkMonitorWindow.h"
#include "ui_NetworkMonitorWindow.h"
#include "databasemanager.h"
#include <QTimer>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

QT_CHARTS_USE_NAMESPACE

NetworkMonitorWindow::NetworkMonitorWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::NetworkMonitorWindow), realtimeTimer(new QTimer(this))
{
    ui->setupUi(this);
    
    setupUiElements();
    setupCharts();
    loadDeviceList();

    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NetworkMonitorWindow::onDeviceChanged);
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &NetworkMonitorWindow::onTimeRangeChanged);
    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &NetworkMonitorWindow::onTimeRangeChanged);
    connect(ui->exportButton, &QPushButton::clicked, this, &NetworkMonitorWindow::onExportClicked);
    connect(realtimeTimer, &QTimer::timeout, this, &NetworkMonitorWindow::refreshRealtimeData);
    
    realtimeTimer->start(3000); // 3秒刷新一次
}

NetworkMonitorWindow::~NetworkMonitorWindow()
{
    delete ui;
}

void NetworkMonitorWindow::setupUiElements()
{
    // 设置时间范围为最近一小时
    ui->endDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->startDateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(-3600));

    // 设置历史数据表格
    ui->historyTable->setColumnCount(4);
    ui->historyTable->setHorizontalHeaderLabels({"时间戳", "温度 (°C)", "湿度 (%)", "光照 (lux)"});
    ui->historyTable->horizontalHeader()->setStretchLastSection(true);
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void NetworkMonitorWindow::setupCharts()
{
    // --- 实时图表设置 ---
    realtimeChart = new QChart();
    realtimeChart->setTitle("实时监控数据");
    realtimeChartView = new QChartView(realtimeChart, this);
    realtimeChartView->setRenderHint(QPainter::Antialiasing);
    ui->realtimeChartWidget->setLayout(new QVBoxLayout());
    ui->realtimeChartWidget->layout()->addWidget(realtimeChartView);

    tempSeriesRealtime = new QLineSeries();
    tempSeriesRealtime->setName("温度");
    humiditySeriesRealtime = new QLineSeries();
    humiditySeriesRealtime->setName("湿度");
    lightSeriesRealtime = new QLineSeries();
    lightSeriesRealtime->setName("光照");

    realtimeChart->addSeries(tempSeriesRealtime);
    realtimeChart->addSeries(humiditySeriesRealtime);
    realtimeChart->addSeries(lightSeriesRealtime);

    QDateTimeAxis *axisXRealtime = new QDateTimeAxis;
    axisXRealtime->setTickCount(10);
    axisXRealtime->setFormat("hh:mm:ss");
    realtimeChart->addAxis(axisXRealtime, Qt::AlignBottom);
    tempSeriesRealtime->attachAxis(axisXRealtime);
    humiditySeriesRealtime->attachAxis(axisXRealtime);
    lightSeriesRealtime->attachAxis(axisXRealtime);

    QValueAxis *axisYRealtime = new QValueAxis;
    axisYRealtime->setLabelFormat("%f");
    realtimeChart->addAxis(axisYRealtime, Qt::AlignLeft);
    tempSeriesRealtime->attachAxis(axisYRealtime);
    humiditySeriesRealtime->attachAxis(axisYRealtime);
    lightSeriesRealtime->attachAxis(axisYRealtime);

    // --- 历史图表设置 ---
    historyChart = new QChart();
    historyChart->setTitle("历史监控数据");
    historyChartView = new QChartView(historyChart, this);
    historyChartView->setRenderHint(QPainter::Antialiasing);
    ui->historyChartWidget->setLayout(new QVBoxLayout());
    ui->historyChartWidget->layout()->addWidget(historyChartView);

    tempSeriesHistory = new QLineSeries();
    tempSeriesHistory->setName("温度");
    humiditySeriesHistory = new QLineSeries();
    humiditySeriesHistory->setName("湿度");
    lightSeriesHistory = new QLineSeries();
    lightSeriesHistory->setName("光照");

    historyChart->addSeries(tempSeriesHistory);
    historyChart->addSeries(humiditySeriesHistory);
    historyChart->addSeries(lightSeriesHistory);
    
    QDateTimeAxis *axisXHistory = new QDateTimeAxis;
    axisXHistory->setTickCount(10);
    axisXHistory->setFormat("MM-dd hh:mm");
    historyChart->addAxis(axisXHistory, Qt::AlignBottom);
    tempSeriesHistory->attachAxis(axisXHistory);
    humiditySeriesHistory->attachAxis(axisXHistory);
    lightSeriesHistory->attachAxis(axisXHistory);

    QValueAxis *axisYHistory = new QValueAxis;
    axisYHistory->setLabelFormat("%f");
    historyChart->addAxis(axisYHistory, Qt::AlignLeft);
    tempSeriesHistory->attachAxis(axisYHistory);
    humiditySeriesHistory->attachAxis(axisYHistory);
    lightSeriesHistory->attachAxis(axisYHistory);
}


void NetworkMonitorWindow::loadDeviceList()
{
    ui->deviceComboBox->clear();
    ui->deviceComboBox->addItem("请选择设备", -1);
    QVariantList devices = DatabaseManager::instance().getDevices();
    for (const QVariant& deviceVariant : devices) {
        QVariantMap device = deviceVariant.toMap();
        ui->deviceComboBox->addItem(device["name"].toString(), device["device_id"]);
    }
}

void NetworkMonitorWindow::onDeviceChanged(int index)
{
    if (index <= 0) { // "请选择设备"
        tempSeriesRealtime->clear();
        humiditySeriesRealtime->clear();
        lightSeriesRealtime->clear();
        updateHistoryUi({});
        return;
    }
    
    // 设备切换时，清空实时数据并立即查询一次历史和实时数据
    tempSeriesRealtime->clear();
    humiditySeriesRealtime->clear();
    lightSeriesRealtime->clear();

    refreshRealtimeData();
    queryHistoryData();
}

void NetworkMonitorWindow::onTimeRangeChanged()
{
    queryHistoryData();
}

void NetworkMonitorWindow::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出历史数据", "", "CSV 文件 (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入。");
        return;
    }

    QTextStream out(&file);
    // 写入表头
    QStringList headers;
    for (int i = 0; i < ui->historyTable->columnCount(); ++i) {
        headers << ui->historyTable->horizontalHeaderItem(i)->text();
    }
    out << headers.join(',') << "\n";

    // 写入数据
    for (int row = 0; row < ui->historyTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < ui->historyTable->columnCount(); ++col) {
            rowData << ui->historyTable->item(row, col)->text();
        }
        out << rowData.join(',') << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "数据已成功导出。");
}

void NetworkMonitorWindow::refreshRealtimeData()
{
    int deviceId = ui->deviceComboBox->currentData().toInt();
    if (deviceId == -1) return;

    // 假设我们有一个函数可以获取最新的单条数据
    QVariantList latestDataList = DatabaseManager::instance().getDeviceData(deviceId, QDateTime::currentDateTime().addSecs(-5), QDateTime::currentDateTime());
    if (!latestDataList.isEmpty()) {
        updateRealtimeChart(latestDataList.last().toMap());
    }
}

void NetworkMonitorWindow::queryHistoryData()
{
    int deviceId = ui->deviceComboBox->currentData().toInt();
    if (deviceId == -1) {
        updateHistoryUi({});
        return;
    };

    QDateTime startTime = ui->startDateTimeEdit->dateTime();
    QDateTime endTime = ui->endDateTimeEdit->dateTime();

    QVariantList historyData = DatabaseManager::instance().getDeviceData(deviceId, startTime, endTime);
    updateHistoryUi(historyData);
}

void NetworkMonitorWindow::updateRealtimeChart(const QVariantMap &data)
{
    if (data.isEmpty()) return;
    
    qint64 timestamp = QDateTime::fromString(data["timestamp"].toString(), Qt::ISODate).toMSecsSinceEpoch();
    
    tempSeriesRealtime->append(timestamp, data["temperature"].toDouble());
    humiditySeriesRealtime->append(timestamp, data["humidity"].toDouble());
    lightSeriesRealtime->append(timestamp, data["light"].toDouble());
    
    // 保持图表中数据点数量，避免无限增长
    if (tempSeriesRealtime->count() > 100) {
        tempSeriesRealtime->remove(0);
        humiditySeriesRealtime->remove(0);
        lightSeriesRealtime->remove(0);
    }
    
    if (tempSeriesRealtime->count() == 0) return;

    realtimeChart->axes(Qt::Horizontal).first()->setRange(
        QDateTime::fromMSecsSinceEpoch(tempSeriesRealtime->at(0).x()),
        QDateTime::fromMSecsSinceEpoch(tempSeriesRealtime->at(tempSeriesRealtime->count() - 1).x())
    );
    
    // 自动调整Y轴范围
    double minVal = 0, maxVal = 100; // 默认范围
    if (tempSeriesRealtime->count() > 0) {
        minVal = tempSeriesRealtime->at(0).y();
        maxVal = tempSeriesRealtime->at(0).y();
        for (const auto& point : tempSeriesRealtime->points()) {
            if (point.y() < minVal) minVal = point.y();
            if (point.y() > maxVal) maxVal = point.y();
        }
    }
     realtimeChart->axes(Qt::Vertical).first()->setRange(minVal - 10, maxVal + 10);
}

void NetworkMonitorWindow::updateHistoryUi(const QVariantList &data)
{
    // 更新表格
    ui->historyTable->setRowCount(0);
    for (int i = 0; i < data.size(); ++i) {
        QVariantMap rowData = data[i].toMap();
        ui->historyTable->insertRow(i);
        ui->historyTable->setItem(i, 0, new QTableWidgetItem(rowData["timestamp"].toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        ui->historyTable->setItem(i, 1, new QTableWidgetItem(QString::number(rowData["temperature"].toDouble())));
        ui->historyTable->setItem(i, 2, new QTableWidgetItem(QString::number(rowData["humidity"].toDouble())));
        ui->historyTable->setItem(i, 3, new QTableWidgetItem(QString::number(rowData["light"].toDouble())));
    }

    // 更新图表
    tempSeriesHistory->clear();
    humiditySeriesHistory->clear();
    lightSeriesHistory->clear();
    if(data.isEmpty()) {
        historyChart->axes(Qt::Horizontal).first()->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addDays(1));
        historyChart->axes(Qt::Vertical).first()->setRange(0, 100);
        return;
    }

    double minVal = 0, maxVal = 100;
    
    for (const QVariant& rowVariant : data) {
        QVariantMap rowData = rowVariant.toMap();
        qint64 timestamp = rowData["timestamp"].toDateTime().toMSecsSinceEpoch();
        double temp = rowData["temperature"].toDouble();
        
        if (tempSeriesHistory->count() == 0) {
            minVal = maxVal = temp;
        } else {
            if (temp < minVal) minVal = temp;
            if (temp > maxVal) maxVal = temp;
        }

        tempSeriesHistory->append(timestamp, temp);
        humiditySeriesHistory->append(timestamp, rowData["humidity"].toDouble());
        lightSeriesHistory->append(timestamp, rowData["light"].toDouble());
    }
    
    historyChart->axes(Qt::Horizontal).first()->setRange(
        data.first().toMap()["timestamp"].toDateTime(),
        data.last().toMap()["timestamp"].toDateTime()
    );
    historyChart->axes(Qt::Vertical).first()->setRange(minVal - 10, maxVal + 10);
}
