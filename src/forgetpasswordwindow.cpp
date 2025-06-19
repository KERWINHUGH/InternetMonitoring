#include "forgetpasswordwindow.h"
#include "ui_forgetpasswordwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include "databasemanager.h"

ForgetPasswordWindow::ForgetPasswordWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ForgetPasswordWindow)
{
    ui->setupUi(this);
    setWindowTitle("找回密码");
    setFixedSize(400, 300);
    connect(ui->btnReset, &QPushButton::clicked, this, &ForgetPasswordWindow::onResetClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &ForgetPasswordWindow::onBackClicked);
}

ForgetPasswordWindow::~ForgetPasswordWindow()
{
    delete ui;
}

void ForgetPasswordWindow::onResetClicked()
{
    QString username = ui->editUsername->text().trimmed();
    QString email = ui->editEmail->text().trimmed();
    QString phone = ui->editPhone->text().trimmed();
    
    if (username.isEmpty() || email.isEmpty() || phone.isEmpty()) {
        showError("请填写完整信息");
        return;
    }
    
    // 先获取user_id
    int user_id;
    if (!DatabaseManager::instance().getUserIdByUsername(username, user_id)) {
        showError("用户名不存在");
        return;
    }
    // 验证用户信息
    QString storedUsername, storedEmail, storedPhone, nickname, role;
    if (!DatabaseManager::instance().getUserInfo(user_id, storedUsername, storedEmail, storedPhone, nickname, role)) {
        showError("用户信息获取失败");
        return;
    }
    if (storedEmail != email || storedPhone != phone) {
        showError("邮箱或手机号不匹配");
        return;
    }
    // 弹出新密码输入对话框
    bool ok;
    QString newPassword = QInputDialog::getText(this, "设置新密码", 
                                               "请输入新密码（6-20位）：", 
                                               QLineEdit::Password, "", &ok);
    if (!ok || newPassword.isEmpty()) {
        return;
    }
    if (newPassword.length() < 6 || newPassword.length() > 20) {
        showError("密码长度必须在6-20位之间");
        return;
    }
    // 更新密码
    if (DatabaseManager::instance().updatePassword(user_id, newPassword)) {
        QMessageBox::information(this, "成功", "密码更新成功！");
        emit backToLogin();
    } else {
        showError("密码更新失败，请重试");
    }
}

void ForgetPasswordWindow::onBackClicked()
{
    emit backToLogin();
}

void ForgetPasswordWindow::showError(const QString& msg)
{
    QMessageBox::warning(this, "找回密码失败", msg);
} 
