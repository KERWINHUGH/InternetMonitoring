#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include "loginmanager.h"

// 前向声明
class AdminWindow;
class UserWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void Init(void);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLoginSuccess(bool isAdmin);
    void onLoginFailed(const QString& errorMsg);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& errorMsg);
    void onAdminWindowClosed();
    void onUserWindowClosed();
    void onMaximizeClicked();

private:
    void showAdminWindow();
    void showUserWindow();
    void clearInputFields();

private:
    Ui::MainWindow *ui;
    LoginManager *logManager;  // 登录管理器
    AdminWindow *adminWindow;  // 管理员窗口
    UserWindow *userWindow;    // 用户窗口
    bool role;
    // 窗口拖拽
    bool m_dragging = false;    //窗口移动标志
    QPoint m_dragPosition;      //窗口移动位置
    // 窗口最大化
    bool m_isMaximized = false; //窗口最大化标志
    QRect m_normalGeometry;     //窗口正常大小
};
#endif // MAINWINDOW_H
