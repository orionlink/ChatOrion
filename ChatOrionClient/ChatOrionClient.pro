QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    global.cpp \
    login_gui.cpp \
    main.cpp \
    mainwindow.cpp \
    network/http_mgr.cpp

HEADERS += \
    global.h \
    login_gui.h \
    mainwindow.h \
    network/http_mgr.h \
    singleton.h

FORMS += \
    login_gui.ui \
    mainwindow.ui

DESTDIR = $$PWD/bin

win32:CONFIG(release, debug | release) {
    # Windows 环境配置
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, \\)  # 将路径中的 '/' 替换为 '\\'

    # 获取输出目录并转换路径分隔符
    OutputDir = $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)  # 将路径中的 '/' 替换为 '\\'

    # 在 Windows 上使用 `copy` 命令
    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
}

unix:CONFIG(release, debug | release) {
    # Linux 环境配置
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, /)  # Linux 本来就使用 '/'，不需要替换

    # 获取输出目录并保持 Linux 格式
    OutputDir = $${OUT_PWD}/$${DESTDIR}

    # 输出调试信息
    message("TargetConfig: $$TargetConfig")
    message("OutputDir: $$OutputDir")

    # 在 Linux 上使用 `cp` 命令
    QMAKE_POST_LINK += cp -f \"$$TargetConfig\" \"$$OutputDir\"
}



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pic.qrc \
    qss.qrc
