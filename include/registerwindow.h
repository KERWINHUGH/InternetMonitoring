#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>
#include "loginmanager.h"

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();
    void setLoginManager(LoginManager* manager);

signals:
    void registerSuccess();
    void registerFailed(const QString& errorMsg);
    void backToLogin();

private slots:
    void onRegisterClicked();
    void onBackClicked();
    void onRegisterResult(bool success, const QString& errorMsg = QString());

private:
    Ui::RegisterWindow *ui;
    LoginManager* loginManager = nullptr;
    void showError(const QString& msg);
};

#endif // REGISTERWINDOW_H 