cmake_minimum_required(VERSION 3.16)

project(InternetMonitoring VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt包
find_package(Qt5 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    Sql
)

# 设置源文件
set(SOURCES
    adminwindow.cpp
    databasemanager.cpp
    loginmanager.cpp
    main.cpp
    mainwindow.cpp
    userwindow.cpp
    registerwindow.cpp
    forgetpasswordwindow.cpp
)

# 设置头文件
set(HEADERS
    adminwindow.h
    databasemanager.h
    loginmanager.h
    mainwindow.h
    userwindow.h
    registerwindow.h
    forgetpasswordwindow.h
)

# 设置UI文件
set(FORMS
    mainwindow.ui
    adminwindow.ui
    userwindow.ui
    registerwindow.ui
    forgetpasswordwindow.ui
)

# 设置资源文件
set(RESOURCES
    img/img.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS} 
    ${FORMS}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
) 