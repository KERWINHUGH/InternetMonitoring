#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

// 前向声明
class ProfileWindow;
class DataViewer;

namespace Ui {
class UserWindow;
}

class UserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserWindow(const QString& username = QString(), QWidget *parent = nullptr);
    ~UserWindow() override;

signals:
    void windowClosed();  // 窗口关闭信号

private slots:
    void onMonitoringClicked();
    void onPersonalSettingsClicked();
    void onLogoutClicked();
    void onCloseClicked();
    void onProfileUpdated();

private:
    Ui::UserWindow *ui;
    QString currentUsername;
    ProfileWindow *profileWindow;
    DataViewer *dataViewer;
};

#endif // USERWINDOW_H 