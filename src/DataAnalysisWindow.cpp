#include "DataAnalysisWindow.h"
#include "ui_DataAnalysisWindow.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

DataAnalysisWindow::DataAnalysisWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataAnalysisWindow)
{
    ui->setupUi(this);
    setupUiElements();
    setupChart();
    loadDeviceList();

    connect(ui->analysisButton, &QPushButton::clicked, this, &DataAnalysisWindow::onAnalysisClicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &DataAnalysisWindow::onExportClicked);
}

DataAnalysisWindow::~DataAnalysisWindow()
{
    delete ui;
}

void DataAnalysisWindow::setupUiElements()
{
    // 填充数据类型下拉框
    ui->typeComboBox->addItem("温度", "temperature");
    ui->typeComboBox->addItem("湿度", "humidity");
    ui->typeComboBox->addItem("光照", "light");

    // 设置时间范围为最近一天
    ui->endDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->startDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));

    // 设置结果表格
    ui->resultTable->setColumnCount(4);
    ui->resultTable->setHorizontalHeaderLabels({"设备名称", "最大值", "最小值", "平均值"});
    ui->resultTable->horizontalHeader()->setStretchLastSection(true);
    ui->resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DataAnalysisWindow::setupChart()
{
    chart = new QChart();
    chart->setTitle("数据分析结果");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->chartWidget->setLayout(new QVBoxLayout());
    ui->chartWidget->layout()->addWidget(chartView);

    series = new QBarSeries();
    chart->addSeries(series);

    // X轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Y轴
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
}

void DataAnalysisWindow::loadDeviceList()
{
    ui->deviceComboBox->clear();
    ui->deviceComboBox->addItem("所有设备", -1);
    QVariantList devices = DatabaseManager::instance().getDevices();
    for (const QVariant& deviceVariant : devices) {
        QVariantMap device = deviceVariant.toMap();
        ui->deviceComboBox->addItem(device["name"].toString(), device["device_id"]);
    }
}

void DataAnalysisWindow::onAnalysisClicked()
{
    int deviceId = ui->deviceComboBox->currentData().toInt();
    QString dataType = ui->typeComboBox->currentData().toString();
    QDateTime startTime = ui->startDateTimeEdit->dateTime();
    QDateTime endTime = ui->endDateTimeEdit->dateTime();

    performAnalysis(deviceId, dataType, startTime, endTime);
}

void DataAnalysisWindow::performAnalysis(int deviceId, const QString& dataType, const QDateTime& startTime, const QDateTime& endTime)
{
    QList<QVariantMap> analysisResult;
    QList<int> deviceIds;

    if (deviceId == -1) { // 分析所有设备
        QVariantList devices = DatabaseManager::instance().getDevices();
        for(const auto& device : devices) {
            deviceIds.append(device.toMap()["device_id"].toInt());
        }
    } else {
        deviceIds.append(deviceId);
    }
    
    for(int id : deviceIds) {
        QVariantList monitorData = DatabaseManager::instance().getDeviceData(id, startTime, endTime);
        if (monitorData.isEmpty()) continue;

        double maxVal = monitorData.first().toMap()[dataType].toDouble();
        double minVal = maxVal;
        double sum = 0;

        for (const QVariant& dataPoint : monitorData) {
            double value = dataPoint.toMap()[dataType].toDouble();
            if (value > maxVal) maxVal = value;
            if (value < minVal) minVal = value;
            sum += value;
        }
        
        double avgVal = sum / monitorData.size();

        QVariantMap result;
        QVariantMap deviceDetails;
        DatabaseManager::instance().getDeviceById(id, deviceDetails);
        result["device_name"] = deviceDetails["name"].toString();
        result["max"] = maxVal;
        result["min"] = minVal;
        result["avg"] = avgVal;
        analysisResult.append(result);
    }
    
    updateTable(analysisResult);
    updateChart(analysisResult);
}

void DataAnalysisWindow::updateTable(const QList<QVariantMap>& analysisResult)
{
    ui->resultTable->setRowCount(0);
    int row = 0;
    for (const auto& result : analysisResult) {
        ui->resultTable->insertRow(row);
        ui->resultTable->setItem(row, 0, new QTableWidgetItem(result["device_name"].toString()));
        ui->resultTable->setItem(row, 1, new QTableWidgetItem(QString::number(result["max"].toDouble(), 'f', 2)));
        ui->resultTable->setItem(row, 2, new QTableWidgetItem(QString::number(result["min"].toDouble(), 'f', 2)));
        ui->resultTable->setItem(row, 3, new QTableWidgetItem(QString::number(result["avg"].toDouble(), 'f', 2)));
        row++;
    }
}

void DataAnalysisWindow::updateChart(const QList<QVariantMap>& analysisResult)
{
    chart->removeAllSeries();
    
    // 移除旧的X轴
    QList<QAbstractAxis*> oldAxes = chart->axes(Qt::Horizontal);
    for(QAbstractAxis* axis : oldAxes) {
        chart->removeAxis(axis);
        delete axis;
    }

    series = new QBarSeries();
    QBarSet *maxSet = new QBarSet("最大值");
    QBarSet *minSet = new QBarSet("最小值");
    QBarSet *avgSet = new QBarSet("平均值");
    QStringList categories;

    double overallMin = 0;
    double overallMax = 0;

    for (int i=0; i < analysisResult.size(); ++i) {
        const auto& result = analysisResult[i];
        double maxV = result["max"].toDouble();
        double minV = result["min"].toDouble();

        if(i==0) {
            overallMin = minV;
            overallMax = maxV;
        } else {
            if(minV < overallMin) overallMin = minV;
            if(maxV > overallMax) overallMax = maxV;
        }
        
        *maxSet << maxV;
        *minSet << minV;
        *avgSet << result["avg"].toDouble();
        categories << result["device_name"].toString();
    }

    series->append(maxSet);
    series->append(minSet);
    series->append(avgSet);
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    series->attachAxis(axisX);
    
    if(!chart->axes(Qt::Vertical).isEmpty())
    {
        auto axisY = static_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
        axisY->setRange(overallMin * 0.9, overallMax * 1.1);
    }
}

void DataAnalysisWindow::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出分析结果", "", "CSV 文件 (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入。");
        return;
    }

    QTextStream out(&file);
    QStringList headers;
    for (int i = 0; i < ui->resultTable->columnCount(); ++i) {
        headers << ui->resultTable->horizontalHeaderItem(i)->text();
    }
    out << headers.join(',') << "\n";

    for (int row = 0; row < ui->resultTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < ui->resultTable->columnCount(); ++col) {
            rowData << ui->resultTable->item(row, col)->text();
        }
        out << rowData.join(',') << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "数据已成功导出。");
} 