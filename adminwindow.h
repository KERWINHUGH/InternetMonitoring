#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow() override;

signals:
    void windowClosed();  // 窗口关闭信号

private slots:
    void onUserManagementClicked();
    void onSystemSettingsClicked();
    void onLogoutClicked();
    void onCloseClicked();

private:
    Ui::AdminWindow *ui;
};

#endif // ADMINWINDOW_H 
