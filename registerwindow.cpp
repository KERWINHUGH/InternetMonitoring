#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QMessageBox>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    setWindowTitle("用户注册");
    setFixedSize(400, 400);
    connect(ui->btnRegister, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &RegisterWindow::onBackClicked);
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::onRegisterClicked()
{
    // 这里应实现注册逻辑，暂用信号模拟
    emit registerSuccess();
}

void RegisterWindow::onBackClicked()
{
    emit backToLogin();
}

void RegisterWindow::showError(const QString& msg)
{
    QMessageBox::warning(this, "注册失败", msg);
} 