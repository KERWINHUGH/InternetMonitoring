//#include <QApplication>
//#include <QDebug>
//#include <QMessageBox>
//#include "../include/databasemanager.h"
//#include "../include/profilewindow.h"
//#include "../include/dataviewer.h"

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);

//    qDebug() << "开始数据库测试...";

//    // 初始化数据库
//    if (!DatabaseManager::instance().initDatabase()) {
//        qDebug() << "数据库初始化失败!";
//        QMessageBox::critical(nullptr, "错误", "数据库初始化失败!");
//        return -1;
//    }

//    qDebug() << "数据库初始化成功!";

//    // 测试用户信息获取
//    QString email, phone, nickname, role;
//    if (DatabaseManager::instance().getUserInfo("jack", email, phone, nickname, role)) {
//        qDebug() << "用户jack信息:";
//        qDebug() << "邮箱:" << email;
//        qDebug() << "手机:" << phone;
//        qDebug() << "昵称:" << nickname;
//        qDebug() << "角色:" << role;
//    } else {
//        qDebug() << "获取用户jack信息失败!";
//    }

//    // 测试用户信息更新
//    if (DatabaseManager::instance().updateUser("jack", "jack_new@example.com", "13800138002", "新昵称")) {
//        qDebug() << "用户jack信息更新成功!";
//    } else {
//        qDebug() << "用户jack信息更新失败!";
//    }

//    // 再次获取用户信息验证更新
//    if (DatabaseManager::instance().getUserInfo("jack", email, phone, nickname, role)) {
//        qDebug() << "更新后用户jack信息:";
//        qDebug() << "邮箱:" << email;
//        qDebug() << "手机:" << phone;
//        qDebug() << "昵称:" << nickname;
//        qDebug() << "角色:" << role;
//    }

//    // 测试添加新用户
//    if (DatabaseManager::instance().addUser("testuser", "password123",
//                                           "test@example.com", "13800138003",
//                                           "测试用户", "user")) {
//        qDebug() << "新用户添加成功!";
//    } else {
//        qDebug() << "新用户添加失败!";
//    }

//    // 测试设备管理
//    if (DatabaseManager::instance().addDevice("TEST001", "测试设备", "sensor", "测试位置")) {
//        qDebug() << "测试设备添加成功!";
//    } else {
//        qDebug() << "测试设备添加失败!";
//    }

//    // 测试监控数据
//    if (DatabaseManager::instance().addMonitorData("TEST001", 25.0, 60.0, 50.0, 70.0, 100.0)) {
//        qDebug() << "监控数据添加成功!";
//    } else {
//        qDebug() << "监控数据添加失败!";
//    }

//    qDebug() << "数据库测试完成!";

//    // 显示测试结果
//    QMessageBox::information(nullptr, "测试完成", "数据库功能测试完成，请查看控制台输出!");

//    return 0;
//}
