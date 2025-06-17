#ifndef FORGETPASSWORDWINDOW_H
#define FORGETPASSWORDWINDOW_H

#include <QMainWindow>

namespace Ui {
class ForgetPasswordWindow;
}

class ForgetPasswordWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ForgetPasswordWindow(QWidget *parent = nullptr);
    ~ForgetPasswordWindow();

signals:
    void passwordResetSuccess();
    void passwordResetFailed(const QString& errorMsg);
    void backToLogin();

private slots:
    void onResetClicked();
    void onBackClicked();

private:
    Ui::ForgetPasswordWindow *ui;
    void showError(const QString& msg);
};

#endif // FORGETPASSWORDWINDOW_H 