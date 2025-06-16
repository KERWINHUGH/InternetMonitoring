#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Init();

}

/**
 * @brief 初始化主窗口界面
 * 
 * 该函数负责初始化主窗口的各种控件和信号槽连接：
 * 1. 连接窗口控制按钮（关闭、最小化、最大化）
 * 2. 设置输入框的占位符文本
 * 3. 连接登录和注册按钮的信号槽
 * 4. 初始化登录尝试次数计数器
 */
void MainWindow::Init()
{
    // 连接窗口控制按钮信号槽
    connect(ui->bntClose, &QPushButton::clicked, this, &QMainWindow::close);        // 关闭按钮
    connect(ui->bntMin, &QPushButton::clicked, this, &QMainWindow::showMinimized);  // 最小化按钮
    connect(ui->bntMax, &QPushButton::clicked, this, &QMainWindow::showMaximized);  // 最大化按钮

    // 设置输入框占位符文本
    ui->editUserAccount->setPlaceholderText("请输入用户名");   // 用户名输入框提示
    ui->editUserPassord->setPlaceholderText("请输入密码");     // 密码输入框提示

    // 连接功能按钮信号槽
    connect(ui->bntLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);     // 登录按钮
    connect(ui->bntRegister, &QPushButton::clicked, this, &MainWindow::onRegisterClicked); // 注册按钮
    
    // 初始化登录尝试次数
    loginAttempts = 0;
}

/**
 * @brief 登录按钮点击事件处理函数
 * 
 * 该函数负责处理用户登录逻辑：
 * 1. 获取用户输入的用户名和密码
 * 2. 验证输入的有效性
 * 3. 检查用户凭据
 * 4. 处理登录成功或失败的情况
 * 5. 限制登录尝试次数（最多3次）
 */
void MainWindow::onLoginClicked()
{
    QString user = ui->editUserAccount->text();    // 获取用户名
    QString pwd = ui->editUserPassord->text();     // 获取密码
    QString errMsg;                                // 错误信息
    bool isAdmin = false;                          // 是否为管理员

    // 验证输入有效性
    if (!checkInputValid(user, pwd, errMsg)) {
        QMessageBox::warning(this, "输入错误", errMsg);
        return;
    }

    // 检查用户凭据
    if (checkUser(user, pwd, isAdmin, errMsg)) {
        loginAttempts = 0;                         // 重置登录尝试次数
        role = isAdmin;                            // 设置用户角色
        QMessageBox::information(this, "登录成功", isAdmin ? "管理员登录成功" : "用户登录成功");
        // TODO: 进入主界面，跳转到管理员或用户界面
    } else {
        loginAttempts++;                           // 增加登录尝试次数
        QMessageBox::warning(this, "登录失败", errMsg);
        // 检查是否超过最大尝试次数
        if (loginAttempts >= 3) {
            QMessageBox::critical(this, "错误", "尝试次数过多，程序将关闭");
            qApp->quit();
        }
    }
}

/**
 * @brief 注册按钮点击事件处理函数
 * 
 * 该函数负责处理用户注册逻辑：
 * 1. 获取用户输入的用户名和密码
 * 2. 验证输入的有效性
 * 3. 注册新用户（默认为普通用户）
 * 4. 显示注册结果
 */
void MainWindow::onRegisterClicked()
{
    QString user = ui->editUserAccount->text();    // 获取用户名
    QString pwd = ui->editUserPassord->text();     // 获取密码
    QString errMsg;                                // 错误信息
    // TODO: 这里可以弹窗让用户选择角色，简单起见假设注册为普通用户
    bool isAdmin = false;                          // 默认为普通用户

    // 验证输入有效性
    if (!checkInputValid(user, pwd, errMsg)) {
        QMessageBox::warning(this, "输入错误", errMsg);
        return;
    }

    // 注册用户
    if (registerUser(user, pwd, isAdmin, errMsg)) {
        QMessageBox::information(this, "注册成功", "请登录");
    } else {
        QMessageBox::warning(this, "注册失败", errMsg);
    }
}

/**
 * @brief 验证用户输入的有效性
 * @param user 用户名
 * @param pwd 密码
 * @param errMsg 错误信息输出参数
 * @return 输入是否有效
 * 
 * 该函数检查：
 * 1. 用户名和密码是否为空
 * 2. 是否包含非法字符（空格、分号等）
 */
bool MainWindow::checkInputValid(const QString& user, const QString& pwd, QString& errMsg)
{
    // 检查是否为空
    if (user.isEmpty() || pwd.isEmpty()) {
        errMsg = "用户名和密码不能为空";
        return false;
    }
    // 检查是否包含非法字符
    if (user.contains(" ") || pwd.contains(" ") || user.contains(";") || pwd.contains(";")) {
        errMsg = "用户名和密码不能包含空格或特殊字符";
        return false;
    }
    return true;
}

/**
 * @brief 检查用户凭据
 * @param user 用户名
 * @param pwd 密码
 * @param isAdmin 是否为管理员输出参数
 * @param errMsg 错误信息输出参数
 * @return 用户凭据是否正确
 * 
 * 该函数从用户文件中读取用户信息并验证：
 * 1. 打开用户数据文件
 * 2. 逐行读取用户信息
 * 3. 验证用户名和密码是否匹配
 * 4. 确定用户角色
 */
bool MainWindow::checkUser(const QString& user, const QString& pwd, bool& isAdmin, QString& errMsg)
{
    QFile file("D:/code/QT/project/InternetMonitoring/users.txt");                       // 用户数据文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errMsg = "无法读取用户数据";
        return false;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();              // 读取一行
        QStringList parts = line.split(",");       // 按逗号分割
        // 检查格式：用户名,密码,角色
        if (parts.size() == 3 && parts[0] == user && parts[1] == pwd) {
            isAdmin = (parts[2] == "admin");       // 确定用户角色
            return true;
        }
    }
    errMsg = "用户名或密码错误";
    return false;
}

/**
 * @brief 注册新用户
 * @param user 用户名
 * @param pwd 密码
 * @param isAdmin 是否为管理员
 * @param errMsg 错误信息输出参数
 * @return 注册是否成功
 * 
 * 该函数负责注册新用户：
 * 1. 检查用户名是否已存在
 * 2. 将新用户信息写入文件
 * 3. 文件格式：用户名,密码,角色
 */
bool MainWindow::registerUser(const QString& user, const QString& pwd, bool isAdmin, QString& errMsg)
{
    QFile file("D:/code/QT/project/InternetMonitoring/users.txt");                       // 用户数据文件
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        errMsg = "无法写入用户数据";
        return false;
    }
    
    // 检查用户名是否已存在
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();              // 读取一行
        QStringList parts = line.split(",");       // 按逗号分割
        if (parts.size() >= 1 && parts[0] == user) {
            errMsg = "用户名已存在";
            return false;
        }
    }
    
    // 写入新用户信息
    QTextStream out(&file);
    out << user << "," << pwd << "," << (isAdmin ? "admin" : "user") << "\n";
    return true;
}

/**
 * @brief 主窗口析构函数
 * 
 * 该函数负责清理资源：
 * 1. 释放UI对象内存
 */
MainWindow::~MainWindow()
{
    delete ui;
}
