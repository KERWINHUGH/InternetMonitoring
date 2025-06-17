#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

signals:
    void registerSuccess();
    void registerFailed(const QString& errorMsg);
    void backToLogin();

private slots:
    void onRegisterClicked();
    void onBackClicked();

private:
    Ui::RegisterWindow *ui;
    void showError(const QString& msg);
};

#endif // REGISTERWINDOW_H 