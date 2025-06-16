#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

signals:
    void windowClosed();  // 窗口关闭信号

private slots:
    void onUserManagementClicked();
    void onSystemSettingsClicked();
    void onLogoutClicked();
    void onCloseClicked();

private:
    void setupUI();
    void createMenuBar();

private:
    QWidget *centralWidget;
    QLabel *welcomeLabel;
    QPushButton *userManagementBtn;
    QPushButton *systemSettingsBtn;
    QPushButton *logoutBtn;
    QPushButton *closeBtn;
};

#endif // ADMINWINDOW_H 