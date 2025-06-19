-- 物联网设备监控系统数据库初始化脚本（新版，2024）

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    phone TEXT UNIQUE NOT NULL,
    nickname TEXT,
    role TEXT NOT NULL
);

-- 设备表
CREATE TABLE IF NOT EXISTS devices (
    device_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    location TEXT NOT NULL,
    manufacturer TEXT,
    model TEXT,
    installation_date DATE
);

-- 数据表
CREATE TABLE IF NOT EXISTS monitor_data (
    data_id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id INTEGER NOT NULL,
    timestamp DATETIME NOT NULL,
    temperature REAL,
    humidity REAL,
    light REAL,
    -- 可扩展字段：如co2、pressure等
    FOREIGN KEY(device_id) REFERENCES devices(device_id)
);

-- 告警规则表
CREATE TABLE IF NOT EXISTS alarm_rules (
    rule_id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id INTEGER NOT NULL,
    description TEXT NOT NULL,
    condition TEXT NOT NULL,
    action TEXT NOT NULL,
    FOREIGN KEY(device_id) REFERENCES devices(device_id)
);

-- 告警记录表
CREATE TABLE IF NOT EXISTS alarm_records (
    alarm_id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id INTEGER NOT NULL,
    timestamp DATETIME NOT NULL,
    content TEXT NOT NULL,
    status TEXT NOT NULL,
    note TEXT,
    FOREIGN KEY(device_id) REFERENCES devices(device_id)
);

-- 系统日志表
CREATE TABLE IF NOT EXISTS system_logs (
    log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME NOT NULL,
    log_type TEXT NOT NULL,
    log_level TEXT NOT NULL,
    content TEXT NOT NULL,
    user_id INTEGER,
    device_id INTEGER,
    FOREIGN KEY(user_id) REFERENCES users(user_id),
    FOREIGN KEY(device_id) REFERENCES devices(device_id)
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