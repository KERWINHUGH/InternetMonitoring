-- 物联网设备监控系统数据库初始化脚本

-- 创建用户表
CREATE TABLE IF NOT EXISTS users (
    username TEXT PRIMARY KEY,
    password TEXT NOT NULL,
    email TEXT,
    phone TEXT,
    nickname TEXT,
    role TEXT NOT NULL
);

-- 创建设备表
CREATE TABLE IF NOT EXISTS devices (
    device_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    location TEXT,
    status TEXT DEFAULT 'offline'
);

-- 创建监控数据表
CREATE TABLE IF NOT EXISTS monitor_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    temperature REAL,
    humidity REAL,
    cpu_usage REAL,
    memory_usage REAL,
    network_speed REAL,
    FOREIGN KEY(device_id) REFERENCES devices(device_id)
);

-- 创建告警表
CREATE TABLE IF NOT EXISTS alarms (
    alarm_id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    type TEXT NOT NULL,
    description TEXT NOT NULL,
    level TEXT NOT NULL,
    status TEXT DEFAULT 'unhandled',
    FOREIGN KEY(device_id) REFERENCES devices(device_id)
);

-- 创建系统日志表
CREATE TABLE IF NOT EXISTS system_logs (
    log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    type TEXT NOT NULL,
    content TEXT NOT NULL,
    username TEXT NOT NULL
);

-- 插入默认管理员账户 (密码: admin123)
INSERT OR IGNORE INTO users (username, password, email, phone, nickname, role) 
VALUES ('admin', '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9', 'admin@example.com', '13800138000', '系统管理员', 'admin');

-- 插入测试用户账户 (密码: user123)
INSERT OR IGNORE INTO users (username, password, email, phone, nickname, role) 
VALUES ('jack', 'a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3', 'jack@example.com', '13800138001', '测试用户', 'user');

-- 插入测试设备
INSERT OR IGNORE INTO devices (device_id, name, type, location, status) 
VALUES ('DEV001', '温度传感器1', 'sensor', '机房A', 'online');

INSERT OR IGNORE INTO devices (device_id, name, type, location, status) 
VALUES ('DEV002', '湿度传感器1', 'sensor', '机房B', 'online');

-- 插入测试监控数据
INSERT OR IGNORE INTO monitor_data (device_id, temperature, humidity, cpu_usage, memory_usage, network_speed) 
VALUES ('DEV001', 25.5, 60.2, 45.3, 67.8, 100.5);

INSERT OR IGNORE INTO monitor_data (device_id, temperature, humidity, cpu_usage, memory_usage, network_speed) 
VALUES ('DEV002', 26.1, 58.9, 52.1, 71.2, 95.3);

-- 插入测试告警
INSERT OR IGNORE INTO alarms (device_id, type, description, level, status) 
VALUES ('DEV001', '温度告警', '温度超过阈值', 'warning', 'unhandled');

-- 插入系统日志
INSERT OR IGNORE INTO system_logs (type, content, username) 
VALUES ('系统初始化', '数据库初始化完成', 'system'); 