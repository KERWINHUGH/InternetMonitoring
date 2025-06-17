#include "profilewindow.h"
#include <QRegExp>
#include <QCryptographicHash>

ProfileWindow::ProfileWindow(const QString& username, QWidget *parent)
    : QMainWindow(parent), currentUsername(username)
{
    setupUI();
    loadUserInfo();
}

ProfileWindow::~ProfileWindow()
{
}

void ProfileWindow::setupUI()
{
    setWindowTitle("个人信息管理");
    setFixedSize(500, 400);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    // 创建标题
    QLabel *titleLabel = new QLabel("个人信息管理", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 创建信息显示区域
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);

    // 用户名（只读）
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    QLabel *usernameTitle = new QLabel("用户名:", this);
    usernameTitle->setFixedWidth(80);
    usernameLabel = new QLabel(this);
    usernameLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px; background-color: #f5f5f5;");
    usernameLayout->addWidget(usernameTitle);
    usernameLayout->addWidget(usernameLabel);
    infoLayout->addLayout(usernameLayout);

    // 邮箱
    QHBoxLayout *emailLayout = new QHBoxLayout();
    QLabel *emailTitle = new QLabel("邮箱:", this);
    emailTitle->setFixedWidth(80);
    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("请输入邮箱地址");
    emailLayout->addWidget(emailTitle);
    emailLayout->addWidget(emailEdit);
    infoLayout->addLayout(emailLayout);

    // 手机号
    QHBoxLayout *phoneLayout = new QHBoxLayout();
    QLabel *phoneTitle = new QLabel("手机号:", this);
    phoneTitle->setFixedWidth(80);
    phoneEdit = new QLineEdit(this);
    phoneEdit->setPlaceholderText("请输入手机号码");
    phoneLayout->addWidget(phoneTitle);
    phoneLayout->addWidget(phoneEdit);
    infoLayout->addLayout(phoneLayout);

    // 昵称
    QHBoxLayout *nicknameLayout = new QHBoxLayout();
    QLabel *nicknameTitle = new QLabel("昵称:", this);
    nicknameTitle->setFixedWidth(80);
    nicknameEdit = new QLineEdit(this);
    nicknameEdit->setPlaceholderText("请输入昵称");
    nicknameLayout->addWidget(nicknameTitle);
    nicknameLayout->addWidget(nicknameEdit);
    infoLayout->addLayout(nicknameLayout);

    // 角色（只读）
    QHBoxLayout *roleLayout = new QHBoxLayout();
    QLabel *roleTitle = new QLabel("角色:", this);
    roleTitle->setFixedWidth(80);
    roleLabel = new QLabel(this);
    roleLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px; background-color: #f5f5f5;");
    roleLayout->addWidget(roleTitle);
    roleLayout->addWidget(roleLabel);
    infoLayout->addLayout(roleLayout);

    mainLayout->addWidget(infoWidget);

    // 创建按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    saveButton = new QPushButton("保存修改", this);
    changePasswordButton = new QPushButton("修改密码", this);
    backButton = new QPushButton("返回", this);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(changePasswordButton);
    buttonLayout->addWidget(backButton);

    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(saveButton, &QPushButton::clicked, this, &ProfileWindow::onSaveClicked);
    connect(changePasswordButton, &QPushButton::clicked, this, &ProfileWindow::onChangePasswordClicked);
    connect(backButton, &QPushButton::clicked, this, &ProfileWindow::onBackClicked);
}

void ProfileWindow::loadUserInfo()
{
    QString email, phone, nickname, role;
    if (DatabaseManager::instance().getUserInfo(currentUsername, email, phone, nickname, role)) {
        currentEmail = email;
        currentPhone = phone;
        currentNickname = nickname;
        currentRole = role;

        // 显示信息
        usernameLabel->setText(currentUsername);
        emailEdit->setText(currentEmail);
        phoneEdit->setText(currentPhone);
        nicknameEdit->setText(currentNickname);
        roleLabel->setText(currentRole == "admin" ? "管理员" : "普通用户");
    } else {
        QMessageBox::warning(this, "错误", "无法加载用户信息");
    }
}

void ProfileWindow::saveUserInfo()
{
    QString newEmail = emailEdit->text().trimmed();
    QString newPhone = phoneEdit->text().trimmed();
    QString newNickname = nicknameEdit->text().trimmed();

    // 验证邮箱格式
    QRegExp emailRegex("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}\\b");
    if (!newEmail.isEmpty() && !emailRegex.exactMatch(newEmail)) {
        QMessageBox::warning(this, "错误", "邮箱格式不正确");
        return;
    }

    // 验证手机号格式
    QRegExp phoneRegex("^1[3-9]\\d{9}$");
    if (!newPhone.isEmpty() && !phoneRegex.exactMatch(newPhone)) {
        QMessageBox::warning(this, "错误", "手机号格式不正确");
        return;
    }

    // 更新用户信息
    if (DatabaseManager::instance().updateUser(currentUsername, newEmail, newPhone, newNickname)) {
        QMessageBox::information(this, "成功", "个人信息更新成功");
        currentEmail = newEmail;
        currentPhone = newPhone;
        currentNickname = newNickname;
        emit profileUpdated();
    } else {
        QMessageBox::warning(this, "错误", "更新失败，请重试");
    }
}

bool ProfileWindow::changePassword()
{
    // 输入原密码
    bool ok;
    QString oldPassword = QInputDialog::getText(this, "修改密码", 
                                               "请输入原密码:", 
                                               QLineEdit::Password, "", &ok);
    if (!ok || oldPassword.isEmpty()) {
        return false;
    }

    // 验证原密码
    QString role;
    if (!DatabaseManager::instance().verifyUser(currentUsername, oldPassword, role)) {
        QMessageBox::warning(this, "错误", "原密码不正确");
        return false;
    }

    // 输入新密码
    QString newPassword = QInputDialog::getText(this, "修改密码", 
                                               "请输入新密码（6-20位）:", 
                                               QLineEdit::Password, "", &ok);
    if (!ok || newPassword.isEmpty()) {
        return false;
    }

    // 验证新密码长度
    if (newPassword.length() < 6 || newPassword.length() > 20) {
        QMessageBox::warning(this, "错误", "新密码长度必须在6-20位之间");
        return false;
    }

    // 确认新密码
    QString confirmPassword = QInputDialog::getText(this, "修改密码", 
                                                   "请再次输入新密码:", 
                                                   QLineEdit::Password, "", &ok);
    if (!ok || confirmPassword.isEmpty()) {
        return false;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "错误", "两次输入的密码不一致");
        return false;
    }

    // 更新密码
    if (DatabaseManager::instance().updatePassword(currentUsername, newPassword)) {
        QMessageBox::information(this, "成功", "密码修改成功");
        return true;
    } else {
        QMessageBox::warning(this, "错误", "密码修改失败，请重试");
        return false;
    }
}

void ProfileWindow::onSaveClicked()
{
    saveUserInfo();
}

void ProfileWindow::onChangePasswordClicked()
{
    changePassword();
}

void ProfileWindow::onBackClicked()
{
    close();
} 