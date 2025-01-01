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

DISTFILES += \
    config.ini

win32:CONFIG(debug, debug | release) {
    # Windows 环境配置
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, \\)  # 将路径中的 '/' 替换为 '\\'

    # 获取输出目录并转换路径分隔符
    OutputDir = $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)  # 将路径中的 '/' 替换为 '\\'

    # 在 Windows 上使用 `copy` 命令
    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
}

unix:CONFIG(debug, debug | release) {
    # Linux 环境配置
    TargetConfig = $$PWD/config.ini  # 当前目录下的 config.ini 文件
    OutputDir = $$DESTDIR  # 输出目录

    # 输出调试信息
    message("TargetConfig: $$TargetConfig")
    message("OutputDir: $$OutputDir")

    # 拷贝 config.ini 到输出目录
    QMAKE_POST_LINK += cp -f "$$TargetConfig" "$$OutputDir"
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pic.qrc \
    qss.qrc
