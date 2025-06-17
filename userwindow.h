#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include "ui_userwindow.h"

class UserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserWindow(QWidget *parent = nullptr);
    ~UserWindow();

signals:
    void windowClosed();  // 窗口关闭信号

private slots:
    void onNetworkMonitorClicked();
    void onPersonalSettingsClicked();
    void onLogoutClicked();
    void onCloseClicked();

private:
    Ui::UserWindow *ui;
};

#endif // USERWINDOW_H 