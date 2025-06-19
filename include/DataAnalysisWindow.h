#ifndef DATAANALYSISWINDOW_H
#define DATAANALYSISWINDOW_H

#include <QMainWindow>

namespace Ui { class DataAnalysisWindow; }

class DataAnalysisWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DataAnalysisWindow(QWidget *parent = nullptr);
    ~DataAnalysisWindow();

private slots:
    void onAnalysisClicked();
    void onExportClicked();

private:
    Ui::DataAnalysisWindow *ui;
    void updateChart();
    void updateTable();
};

#endif // DATAANALYSISWINDOW_H 