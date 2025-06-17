cmake_minimum_required(VERSION 3.16)

project(InternetMonitoring VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt包
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# 设置源文件
set(SOURCES
    adminwindow.cpp
    loginmanager.cpp
    main.cpp
    mainwindow.cpp
    userwindow.cpp
)

# 设置头文件
set(HEADERS
    adminwindow.h
    loginmanager.h
    mainwindow.h
    userwindow.h
)

# 设置UI文件
set(FORMS
    mainwindow.ui
    adminwindow.ui
    userwindow.ui
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})

# 链接Qt库
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 启用Qt的MOC、UIC和RCC
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
) 