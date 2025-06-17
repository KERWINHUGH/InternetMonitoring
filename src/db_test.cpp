//#include <QCoreApplication>
//#include <QDebug>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include <QSqlError>
//#include <QDir>
//#include <QFile>

//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);

//    qDebug() << "=== 数据库连接测试 ===";

//    // 检查SQLite驱动
//    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
//        qDebug() << "错误: SQLite驱动不可用";
//        return -1;
//    }
//    qDebug() << "✓ SQLite驱动可用";

//    // 创建数据库连接
//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    QString dbPath = QDir::currentPath() + "/internetmonitoring.db";
//    db.setDatabaseName(dbPath);
//    qDebug() << "数据库路径:" << dbPath;

//    // 打开数据库
//    if (!db.open()) {
//        qDebug() << "错误: 无法打开数据库:" << db.lastError().text();
//        return -1;
//    }
//    qDebug() << "✓ 数据库连接成功";

//    // 创建测试表
//    QSqlQuery query;
//    if (!query.exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)")) {
//        qDebug() << "错误: 创建测试表失败:" << query.lastError().text();
//        return -1;
//    }
//    qDebug() << "✓ 测试表创建成功";

//    // 插入测试数据
//    if (!query.exec("INSERT OR IGNORE INTO test_table (id, name) VALUES (1, 'test')")) {
//        qDebug() << "错误: 插入测试数据失败:" << query.lastError().text();
//        return -1;
//    }
//    qDebug() << "✓ 测试数据插入成功";

//    // 查询测试数据
//    if (!query.exec("SELECT * FROM test_table")) {
//        qDebug() << "错误: 查询测试数据失败:" << query.lastError().text();
//        return -1;
//    }

//    while (query.next()) {
//        qDebug() << "查询结果:" << query.value(0).toInt() << query.value(1).toString();
//    }
//    qDebug() << "✓ 数据查询成功";

//    // 删除测试表
//    if (!query.exec("DROP TABLE test_table")) {
//        qDebug() << "警告: 删除测试表失败:" << query.lastError().text();
//    } else {
//        qDebug() << "✓ 测试表清理完成";
//    }

//    qDebug() << "=== 数据库测试完成 ===";
//    qDebug() << "数据库文件大小:" << QFile(dbPath).size() << "字节";

//    return 0;
//}
