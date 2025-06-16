#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QWidget>

class LogManager : public QWidget
{
    Q_OBJECT
public:
    explicit LogManager(QWidget *parent = nullptr);

signals:

public:
    int loginAttempts;      //密码输入尝试次数
    bool checkInputValid(const QString& user, const QString& pwd, QString& errMsg);
    bool checkUser(const QString& user, const QString& pwd, bool& isAdmin, QString& errMsg);
    bool registerUser(const QString& user, const QString& pwd, bool isAdmin, QString& errMsg);

};

#endif // LOGMANAGER_H
