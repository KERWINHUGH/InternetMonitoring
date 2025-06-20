#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QWidget>
#include <QString>
#include <QPushButton>

// 前向声明
class ProfileWindow;
class DatabaseViewer;

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
    void onSidebarButtonClicked(int index);
    void onLogoutClicked();
    void onExitClicked();
    void onProfileUpdated();

private:
    Ui::UserWindow *ui;
    QButtonGroup* sideBarGroup;
    QStackedWidget* mainStackedWidget;
    QWidget* sideBarWidget;
    QToolButton *deviceManagementBtn, *networkMonitorBtn, *alarmRuleManagementBtn, *alarmDisplayBtn, *dataAnalysisBtn, *profileBtn;
    QPushButton *logoutBtn, *exitBtn;
    QString currentUsername;
    ProfileWindow *profileWindow;
};

#endif // USERWINDOW_H 