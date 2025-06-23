#ifndef USEREDITDIALOG_H
#define USEREDITDIALOG_H

#include <QDialog>
#include <QString>
#include <QLineEdit>

struct UserInfo {
    QString username;
    QString email;
    QString phone;
    QString nickname;
    QString role;
    QString password;
};

class UserEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit UserEditDialog(QWidget* parent = nullptr);
    void setUserInfo(const QString& username, const QString& email, const QString& phone, const QString& nickname, const QString& role);
    UserInfo getUserInfo() const;
private:
    void setupUI();
    // 控件声明
    QLineEdit* usernameEdit;
    QLineEdit* emailEdit;
    QLineEdit* phoneEdit;
    QLineEdit* nicknameEdit;
    QLineEdit* roleEdit;
    QLineEdit* passwordEdit;
};

#endif // USEREDITDIALOG_H 