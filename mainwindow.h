#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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


private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    int loginAttempts;      //密码输入尝试次数
    bool checkInputValid(const QString& user, const QString& pwd, QString& errMsg);
    bool checkUser(const QString& user, const QString& pwd, bool& isAdmin, QString& errMsg);
    bool registerUser(const QString& user, const QString& pwd, bool isAdmin, QString& errMsg);

private:
    Ui::MainWindow *ui;

    QString *uesrName;
    QString *userPassword;
    bool role;
};
#endif // MAINWINDOW_H
