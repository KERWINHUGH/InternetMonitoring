#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QInputDialog>
#include "databasemanager.h"

class ProfileWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfileWindow(const QString& username, QWidget *parent = nullptr);
    ~ProfileWindow();

signals:
    void profileUpdated();

private slots:
    void onSaveClicked();
    void onChangePasswordClicked();
    void onBackClicked();

private:
    void setupUI();
    void loadUserInfo();
    void saveUserInfo();
    bool changePassword();

    QString currentUsername;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // 信息显示标签
    QLabel *usernameLabel;
    QLabel *emailLabel;
    QLabel *phoneLabel;
    QLabel *nicknameLabel;
    QLabel *roleLabel;
    
    // 编辑控件
    QLineEdit *emailEdit;
    QLineEdit *phoneEdit;
    QLineEdit *nicknameEdit;
    
    // 按钮
    QPushButton *saveButton;
    QPushButton *changePasswordButton;
    QPushButton *backButton;
    
    // 当前用户信息
    QString currentEmail;
    QString currentPhone;
    QString currentNickname;
    QString currentRole;
};

#endif // PROFILEWINDOW_H 