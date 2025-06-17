#ifndef DATAVIEWER_H
#define DATAVIEWER_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include "databasemanager.h"

class DataViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataViewer(const QString& username, const QString& role, QWidget *parent = nullptr);
    ~DataViewer();

private slots:
    void onTableChanged(int index);
    void onRefreshClicked();
    void onExportClicked();
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();

private:
    void setupUI();
    void loadTableData(const QString& tableName);
    void displayUsers();
    void displayDevices();
    void displayMonitorData();
    void displayAlarms();
    void displaySystemLogs();
    void addUser();
    void editUser();
    void deleteUser();
    void addDevice();
    void editDevice();
    void deleteDevice();

    QString currentUsername;
    QString userRole;
    bool isAdmin;
    
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *controlLayout;
    QComboBox *tableComboBox;
    QPushButton *refreshButton;
    QPushButton *exportButton;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QTableWidget *dataTable;
    QLabel *statusLabel;
};

#endif // DATAVIEWER_H 