#include "UserEditDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

UserEditDialog::UserEditDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
}

void UserEditDialog::setupUI()
{
    setWindowTitle("用户信息编辑");
    setMinimumSize(350, 250);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* userLayout = new QHBoxLayout();
    userLayout->addWidget(new QLabel("用户名：", this));
    usernameEdit = new QLineEdit(this);
    userLayout->addWidget(usernameEdit);
    mainLayout->addLayout(userLayout);

    QHBoxLayout* emailLayout = new QHBoxLayout();
    emailLayout->addWidget(new QLabel("邮箱：", this));
    emailEdit = new QLineEdit(this);
    emailLayout->addWidget(emailEdit);
    mainLayout->addLayout(emailLayout);

    QHBoxLayout* phoneLayout = new QHBoxLayout();
    phoneLayout->addWidget(new QLabel("手机号：", this));
    phoneEdit = new QLineEdit(this);
    phoneLayout->addWidget(phoneEdit);
    mainLayout->addLayout(phoneLayout);

    QHBoxLayout* nicknameLayout = new QHBoxLayout();
    nicknameLayout->addWidget(new QLabel("昵称：", this));
    nicknameEdit = new QLineEdit(this);
    nicknameLayout->addWidget(nicknameEdit);
    mainLayout->addLayout(nicknameLayout);

    QHBoxLayout* roleLayout = new QHBoxLayout();
    roleLayout->addWidget(new QLabel("角色：", this));
    roleEdit = new QLineEdit(this);
    roleEdit->setText("user");
    roleLayout->addWidget(roleEdit);
    mainLayout->addLayout(roleLayout);

    QHBoxLayout* pwdLayout = new QHBoxLayout();
    pwdLayout->addWidget(new QLabel("密码：", this));
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    pwdLayout->addWidget(passwordEdit);
    mainLayout->addLayout(pwdLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* okBtn = new QPushButton("确定", this);
    QPushButton* cancelBtn = new QPushButton("取消", this);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &UserEditDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &UserEditDialog::reject);
}

void UserEditDialog::setUserInfo(const QString& username, const QString& email, const QString& phone, const QString& nickname, const QString& role)
{
    usernameEdit->setText(username);
    emailEdit->setText(email);
    phoneEdit->setText(phone);
    nicknameEdit->setText(nickname);
    roleEdit->setText(role);
}

UserInfo UserEditDialog::getUserInfo() const
{
    UserInfo info;
    info.username = usernameEdit->text().trimmed();
    info.email = emailEdit->text().trimmed();
    info.phone = phoneEdit->text().trimmed();
    info.nickname = nicknameEdit->text().trimmed();
    info.role = roleEdit->text().trimmed();
    info.password = passwordEdit->text();
    return info;
} 