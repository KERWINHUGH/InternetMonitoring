# 物联网设备监控系统

## 项目结构

```
InternetMonitoring/
├── src/                    # 源代码文件
│   ├── main.cpp           # 主程序入口
│   ├── mainwindow.cpp     # 主窗口实现
│   ├── adminwindow.cpp    # 管理员窗口实现
│   ├── userwindow.cpp     # 用户窗口实现
│   ├── profilewindow.cpp  # 个人信息管理窗口
│   ├── dataviewer.cpp     # 数据查看和管理窗口
│   ├── databaseviewer.cpp # 数据库查看器
│   ├── databasemanager.cpp # 数据库管理器
│   ├── loginmanager.cpp   # 登录管理器
│   ├── registerwindow.cpp # 注册窗口
│   ├── forgetpasswordwindow.cpp # 忘记密码窗口
│   └── test_database.cpp  # 数据库测试程序
├── include/               # 头文件
│   ├── mainwindow.h
│   ├── adminwindow.h
│   ├── userwindow.h
│   ├── profilewindow.h
│   ├── dataviewer.h
│   ├── databaseviewer.h
│   ├── databasemanager.h
│   ├── loginmanager.h
│   ├── registerwindow.h
│   └── forgetpasswordwindow.h
├── ui/                    # UI文件
│   ├── mainwindow.ui
│   ├── adminwindow.ui
│   ├── userwindow.ui
│   ├── registerwindow.ui
│   └── forgetpasswordwindow.ui
├── resources/             # 资源文件
│   └── img/
│       ├── img.qrc
│       ├── IOT.png
│       ├── IOTmanagement.png
│       └── monitorcenter.png
├── docs/                  # 文档
│   ├── README.md
│   ├── 功能说明.md
│   ├── 数据库使用说明.md
│   └── 数据库初始化脚本.sql
├── InternetMonitoring.pro # Qt项目文件
├── CMakeLists.txt        # CMake配置文件
└── internetmonitoring.db # SQLite数据库文件（运行时生成）
```

## 编译和运行

### 使用Qt Creator
1. 打开 `InternetMonitoring.pro` 文件
2. 配置项目（选择编译器）
3. 点击运行按钮

### 使用命令行
```bash
# 生成Makefile
qmake InternetMonitoring.pro

# 编译项目
make

# 运行程序
./InternetMonitoring
```

### 使用CMake
```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译项目
make

# 运行程序
./InternetMonitoring
```

## 数据库配置

### 自动初始化
程序首次运行时会自动创建SQLite数据库文件 `internetmonitoring.db`，并初始化所有表结构。

### 手动初始化
如果需要手动初始化数据库，可以使用提供的SQL脚本：

```bash
# 使用sqlite3命令行工具
sqlite3 internetmonitoring.db < docs/数据库初始化脚本.sql
```

### 默认账户
- **管理员账户**: admin / admin123
- **测试用户账户**: jack / user123

## 功能测试

### 数据库功能测试
运行测试程序验证数据库功能：

```bash
# 编译测试程序
g++ -o test_db src/test_database.cpp src/databasemanager.cpp -I include/ -lQt5Core -lQt5Sql -lQt5Widgets

# 运行测试
./test_db
```

### 功能验证清单
- [ ] 用户登录功能
- [ ] 用户注册功能
- [ ] 忘记密码功能
- [ ] 个人信息查看和修改
- [ ] 密码修改功能
- [ ] 数据查看功能（普通用户）
- [ ] 数据管理功能（管理员）
- [ ] 权限控制功能

## 常见问题

### 1. 编译错误
- 确保已安装Qt开发环境
- 检查Qt版本兼容性
- 确认所有依赖库已安装

### 2. 数据库连接失败
- 检查程序是否有写入权限
- 确认SQLite驱动已安装
- 查看错误日志信息

### 3. 功能异常
- 检查数据库文件是否存在
- 验证用户权限设置
- 查看系统日志信息

## 开发环境

- **Qt版本**: 5.12 或更高
- **编译器**: MSVC, MinGW, GCC
- **数据库**: SQLite 3
- **操作系统**: Windows, Linux, macOS

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 联系方式

如有问题或建议，请联系开发团队。 