#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QMessageBox>
#include "loginmanager.h"

RegisterWindow::RegisterWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    setWindowTitle("用户注册");
    setFixedSize(400, 400);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->btnRegister, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &RegisterWindow::onBackClicked);
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::onRegisterClicked()
{
    if (!loginManager) {
        showError("未设置注册管理器");
        return;
    }
    QString username = ui->editUsername->text().trimmed();
    QString password = ui->editPassword->text();
    QString email = ui->editEmail->text().trimmed();
    QString phone = ui->editPhone->text().trimmed();
    // 先断开旧的信号连接，防止重复弹窗
    disconnect(loginManager, &LoginManager::registerSuccess, this, nullptr);
    disconnect(loginManager, &LoginManager::registerFailed, this, nullptr);
    connect(loginManager, &LoginManager::registerSuccess, this, [this]() {
        disconnect(loginManager, &LoginManager::registerSuccess, this, nullptr);
        disconnect(loginManager, &LoginManager::registerFailed, this, nullptr);
        this->close(); // 注册成功后只关闭注册窗口，不弹窗
        emit registerSuccess();
    });
    connect(loginManager, &LoginManager::registerFailed, this, [this](const QString& msg) {
        disconnect(loginManager, &LoginManager::registerSuccess, this, nullptr);
        disconnect(loginManager, &LoginManager::registerFailed, this, nullptr);
        showError(msg);
        // 注册失败不退出程序
    });
    loginManager->handleRegister(username, password, email, phone, false);
}

void RegisterWindow::onBackClicked()
{
    emit backToLogin();
}

void RegisterWindow::showError(const QString& msg)
{
    QMessageBox::warning(this, tr("注册失败"), msg, QMessageBox::Ok, QMessageBox::Ok);
}

void RegisterWindow::setLoginManager(LoginManager* manager)
{
    loginManager = manager;
}

void RegisterWindow::onRegisterResult(bool success, const QString& errorMsg)
{
    if (success) {
        emit registerSuccess();
    } else {
        showError(errorMsg);
        emit registerFailed(errorMsg);
    }
} 