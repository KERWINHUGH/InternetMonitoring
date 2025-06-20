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
#include <QSqlDatabase>
#include <QRegularExpression>
#include <QMap>
#include <QVariantMap>

class DatabaseViewer : public QMainWindow
{
    Q_OBJECT

public:
//    explicit DatabaseViewer(QWidget *parent = nullptr);
    explicit DatabaseViewer(QWidget *parent = nullptr, const QStringList& tables = QStringList(), bool readonly = false);
    ~DatabaseViewer();

private slots:
    void onTableChanged();
    void onRefreshClicked();
    void onExportClicked();
    void onAddClicked();
    void onDeleteClicked();
    void onSaveChangesClicked();
    void onCellChanged(int row, int column);

private:
    void setupUI();
    void loadTableData(const QString& tableName);
    void displayUsers();
    void displayDevices();
    void displayMonitorData();
    void displayAlarms();
    void displaySystemLogs();

    void displayAlarmRules();
    void displayAlarmRecords();
    void displayDeviceGroups();

    QWidget *centralWidget;     // 主窗口
    QVBoxLayout *mainLayout; // 主布局
    QHBoxLayout *controlLayout; // 控制布局
    QLabel *tableLabel; // "选择表"标签
    QComboBox *tableComboBox; // 表选择下拉框
    QPushButton *refreshButton; // 刷新按钮
    QPushButton *exportButton; // 导出按钮
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QTableWidget *dataTable; // 数据表格
    QLabel *statusLabel; // 状态标签
    QStringList customTables;   // 自定义表
    QMap<int, QVariantMap> changedRows; // 跟踪已更改的行
    bool m_readonly = false;
};

#endif // DATABASEVIEWER_H 
