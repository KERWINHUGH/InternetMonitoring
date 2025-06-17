#ifndef DATABASEVIEWER_H
#define DATABASEVIEWER_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "databasemanager.h"

class DatabaseViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit DatabaseViewer(QWidget *parent = nullptr);
    ~DatabaseViewer();

private slots:
    void onTableChanged(int index);
    void onRefreshClicked();
    void onExportClicked();

private:
    void setupUI();
    void loadTableData(const QString& tableName);
    void displayUsers();
    void displayDevices();
    void displayMonitorData();
    void displayAlarms();
    void displaySystemLogs();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *controlLayout;
    QComboBox *tableComboBox;
    QPushButton *refreshButton;
    QPushButton *exportButton;
    QTableWidget *dataTable;
    QLabel *statusLabel;
};

#endif // DATABASEVIEWER_H 