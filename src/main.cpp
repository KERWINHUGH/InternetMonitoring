#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化数据库
    if (!DatabaseManager::instance().initDatabase()) {
        qDebug() << "数据库初始化失败!";
        return -1;
    }

    // 检查并创建初始管理员账户
    int admin_id;
    QString username, email, phone, nickname, role;
    if (!DatabaseManager::instance().getUserIdByUsername("admin", admin_id) ||
        !DatabaseManager::instance().getUserInfo(admin_id, username, email, phone, nickname, role)) {
        // 管理员账户不存在，创建一个
        if (!DatabaseManager::instance().addUser(
            "admin", "admin123", 
            "admin@example.com", 
            "13800138000", 
            "系统管理员",
            "admin"
        )) {
            qDebug() << "创建管理员账户失败!";
            return -1;
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
