QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/adminwindow.cpp \
    src/databasemanager.cpp \
    src/databaseviewer.cpp \
    src/forgetpasswordwindow.cpp \
    src/loginmanager.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/profilewindow.cpp \
    src/registerwindow.cpp \
    src/userwindow.cpp \
    src/DeviceManagementWindow.cpp \
    src/DeviceEditDialog.cpp \
    src/NetworkMonitorWindow.cpp \
    src/AlarmRuleManagementWindow.cpp \
    src/AlarmDisplayWindow.cpp \
    src/DataAnalysisWindow.cpp \
    src/UserEditDialog.cpp \
    src/alarmruleeditdialog.cpp


HEADERS += \
    include/adminwindow.h \
    include/databasemanager.h \
    include/databaseviewer.h \
    include/forgetpasswordwindow.h \
    include/loginmanager.h \
    include/mainwindow.h \
    include/profilewindow.h \
    include/registerwindow.h \
    include/userwindow.h\
    include/DeviceManagementWindow.h \
    include/DeviceEditDialog.h \
    include/NetworkMonitorWindow.h \
    include/AlarmRuleManagementWindow.h \
    include/AlarmDisplayWindow.h \
    include/DataAnalysisWindow.h \
    include/UserEditDialog.h \
    include/alarmruleeditdialog.h

FORMS += \
    ui/AlarmDisplayPage.ui \
    ui/AlarmRuleManagementPage.ui \
    ui/DataAnalysisPage.ui \
    ui/DeviceManagementPage.ui \
    ui/DeviceManagementWindow.ui \
    ui/NetworkMonitorPage.ui \
    ui/NetworkMonitorWindow.ui \
    ui/ProfileWindow.ui \
    ui/SystemSettingsDialog.ui \
    ui/SystemSettingsPage.ui \
    ui/forgetpasswordwindow.ui \
    ui/mainwindow.ui \
    ui/adminwindow.ui \
    ui/registerwindow.ui \
    ui/userwindow.ui \
    ui/AlarmRuleManagementWindow.ui \
    ui/AlarmDisplayWindow.ui \
    ui/DataAnalysisWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/img/img.qrc

# 包含目录
INCLUDEPATH += include/
