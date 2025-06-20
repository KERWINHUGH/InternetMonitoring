# 物联网设备监控系统

## 项目简介

本系统是基于 Qt 和 SQLite 的物联网设备监控与管理平台，支持管理员和普通用户两种角色，具备设备管理、网络监控、告警规则、数据分析、系统日志、权限控制等功能。界面采用现代化侧边栏风格，支持多平台运行。

## 项目结构

```
InternetMonitoring/
├── src/                    # 源代码
│   ├── main.cpp
│   ├── adminwindow.cpp
│   ├── userwindow.cpp
│   ├── profilewindow.cpp
│   ├── databaseviewer.cpp
│   ├── databasemanager.cpp
│   ├── ...（其他功能模块）
├── include/                # 头文件
│   ├── adminwindow.h
│   ├── userwindow.h
│   ├── profilewindow.h
│   ├── databaseviewer.h
│   ├── databasemanager.h
│   ├── ...（其他头文件）
├── ui/                     # Qt Designer UI文件
│   ├── adminwindow.ui
│   ├── userwindow.ui
│   ├── ...（其他UI文件）
├── resources/              # 资源文件
│   └── img/
│       ├── IOT.png
│       ├── IOTmanagement.png
│       └── monitorcenter.png
├── docs/                   # 文档
│   ├── README.md
│   ├── 功能说明.md
│   ├── 数据库使用说明.md
│   └── 数据库初始化脚本.sql
├── InternetMonitoring.pro  # Qt项目文件
├── CMakeLists.txt          # CMake配置文件
└── internetmonitoring.db   # SQLite数据库（运行时生成）
```

## 编译和运行

### 使用Qt Creator
1. 打开 `InternetMonitoring.pro` 文件
2. 配置项目（选择合适的编译器）
3. 构建并运行

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
- **测试用户账户**: jack / Jack123

## 主要功能

### 管理员界面

- 用户管理（增删改查）
- 设备管理（分组、增删改查）
- 网络监控
- 告警规则管理
- 告警展示
- 数据分析
- 系统日志
- 权限控制

### 用户界面

- 设备管理（只读）
- 网络监控（只读）
- 告警规则（只读）
- 告警展示（只读）
- 数据分析（只读）
- 个人设置（可编辑）

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

- **Qt版本**: 5.14.2 
- **编译器**:  MinGW
- **数据库**: SQLite 3
- **操作系统**: Windows
