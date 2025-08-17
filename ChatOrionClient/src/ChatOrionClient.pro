QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    app/main.cpp \
    core/global.cpp \
    core/user_data.cpp \
    core/user_mgr.cpp \
    gui/apply_friend_page.cpp \
    gui/chat_dialog.cpp \
    gui/chat_page.cpp \
    gui/find_failed_dialog.cpp \
    gui/find_success_dialog.cpp \
    gui/friend_info_page.cpp \
    gui/list_item_base.cpp \
    gui/normal_page.cpp \
    gui/self_info_dialog.cpp \
    gui/state_widget.cpp \
    gui/auth/login/login_gui.cpp \
    gui/auth/login/timer_btn.cpp \
    gui/chats/bubble_frame.cpp \
    gui/chats/chat_item_base.cpp \
    gui/chats/chat_user_item.cpp \
    gui/chats/chat_user_list.cpp \
    gui/chats/chat_view.cpp \
    gui/chats/content_bubble.cpp \
    gui/chats/emotion_bubble.cpp \
    gui/chats/emotion_label_item.cpp \
    gui/chats/emotion_window.cpp \
    gui/chats/message_text_edit.cpp \
    gui/chats/picture_bubble.cpp \
    gui/chats/text_bubble.cpp \
    gui/contacts/apply_friend_dialog.cpp \
    gui/contacts/apply_friend_item.cpp \
    gui/contacts/apply_friend_list.cpp \
    gui/contacts/authen_friend_dialog.cpp \
    gui/contacts/con_user_item.cpp \
    gui/contacts/contact_user_list.cpp \
    gui/contacts/group_tip_item.cpp \
    gui/contacts/search_list.cpp \
    gui/contacts/search_user_item.cpp \
    gui/utilities/clicked_btn.cpp \
    gui/utilities/clicked_label.cpp \
    gui/utilities/clicked_once_label.cpp \
    gui/utilities/friend_label.cpp \
    gui/utilities/loading_dlg.cpp \
    gui/utilities/red_dot_label.cpp \
    gui/utilities/slip_button.cpp \
    gui/utilities/customize_edit.cpp \
    network/http_mgr.cpp \
    network/tcp_mgr.cpp \
    utils/base_dialog.cpp \
    utils/common_utils.cpp \
    utils/cui_helper.cpp \
    utils/frameless_wid_drag_mgr.cpp \
    utils/keychainclass.cpp \
    utils/message_bus.cpp \
    utils/tools.cpp

HEADERS += \
    core/global.h \
    core/singleton.h \
    core/user_data.h \
    core/user_mgr.h \
    gui/apply_friend_page.h \
    gui/chat_dialog.h \
    gui/chat_page.h \
    gui/find_failed_dialog.h \
    gui/find_success_dialog.h \
    gui/friend_info_page.h \
    gui/list_item_base.h \
    gui/normal_page.h \
    gui/self_info_dialog.h \
    gui/state_widget.h \
    gui/auth/login/login_gui.h \
    gui/auth/login/timer_btn.h \
    gui/chats/bubble_frame.h \
    gui/chats/chat_item_base.h \
    gui/chats/chat_user_item.h \
    gui/chats/chat_user_list.h \
    gui/chats/chat_view.h \
    gui/chats/content_bubble.h \
    gui/chats/emotion_bubble.h \
    gui/chats/emotion_label_item.h \
    gui/chats/emotion_window.h \
    gui/chats/message_text_edit.h \
    gui/chats/picture_bubble.h \
    gui/chats/text_bubble.h \
    gui/contacts/apply_friend_dialog.h \
    gui/contacts/apply_friend_item.h \
    gui/contacts/apply_friend_list.h \
    gui/contacts/authen_friend_dialog.h \
    gui/contacts/con_user_item.h \
    gui/contacts/contact_user_list.h \
    gui/contacts/group_tip_item.h \
    gui/contacts/search_list.h \
    gui/contacts/search_user_item.h \
    gui/utilities/clicked_btn.h \
    gui/utilities/clicked_label.h \
    gui/utilities/clicked_once_label.h \
    gui/utilities/friend_label.h \
    gui/utilities/loading_dlg.h \
    gui/utilities/red_dot_label.h \
    gui/utilities/slip_button.h \
    gui/utilities/customize_edit.h \
    network/http_mgr.h \
    network/tcp_mgr.h \
    utils/base_dialog.h \
    utils/common_utils.h \
    utils/cui_helper.h \
    utils/frameless_wid_drag_mgr.h \
    utils/keychainclass.h \
    utils/message_bus.h \
    utils/message_commands.h \
    utils/tools.h

FORMS += \
    ui/apply_friend_dialog.ui \
    ui/apply_friend_item.ui \
    ui/apply_friend_page.ui \
    ui/authen_friend_dialog.ui \
    ui/chat_dialog.ui \
    ui/chat_page.ui \
    ui/chat_user_item.ui \
    ui/con_user_item.ui \
    ui/emotion_window.ui \
    ui/find_failed_dialog.ui \
    ui/find_success_dialog.ui \
    ui/friend_info_page.ui \
    ui/friend_label.ui \
    ui/group_tip_item.ui \
    ui/login_gui.ui \
    ui/normal_page.ui \
    ui/search_user_item.ui \
    ui/self_info_dialog.ui

DESTDIR = $$PWD/../bin

INCLUDEPATH += $$PWD/app
INCLUDEPATH += $$PWD/core
INCLUDEPATH += $$PWD/gui
INCLUDEPATH += $$PWD/gui/auth/login
INCLUDEPATH += $$PWD/gui/chats
INCLUDEPATH += $$PWD/gui/contacts
INCLUDEPATH += $$PWD/gui/utilities
INCLUDEPATH += $$PWD/network
INCLUDEPATH += $$PWD/utils

include($$PWD/gui/FrameWgt/FrameWgt.pri)
include($$PWD/qtkeychain.pri)

DISTFILES += \
    config.ini \
    res/pic/normal_logo.png

# Linux 系统
unix {
#    LIBS += -L$$PWD/../third/linux/qtkeychain/lib -lqt5keychain
#    INCLUDEPATH += $$PWD/../third/linux/qtkeychain/include
}

# Windows 系统
win32 {
    LIBS += -lCrypt32
    INCLUDEPATH += $$PWD/qtkeychain
    QMAKE_CXXFLAGS += -utf-8
}

win32-msvc* {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
    QMAKE_CXXFLAGS += -Zm800
}

CONFIG += resources_big
CONFIG += utf8_source
QMAKE_RESOURCE_FLAGS += -no-compress

#win32:CONFIG(debug, debug | release) {
#    # Windows 环境配置
#    TargetConfig = $${PWD}/config.ini
#    TargetEmoji = $$PWD/emoji.json
#    TargetConfig = $$replace(TargetConfig, /, \\)  # 将路径中的 '/' 替换为 '\\'
#    TargetEmoji = $$replace(TargetEmoji, /, \\)

#    # 获取输出目录并转换路径分隔符
#    OutputDir = $${OUT_PWD}/$${DESTDIR}
#    OutputDir = $$replace(OutputDir, /, \\)  # 将路径中的 '/' 替换为 '\\'

#    # 在 Windows 上使用 `copy` 命令
#    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
#    QMAKE_POST_LINK += copy /Y \"$$TargetEmoji\" \"$$OutputDir\"

#    LIBS += -L$$PWD/../third/win/qtkeychain/lib -lqt5keychain
#}

#unix:CONFIG(debug, debug | release) {
#    # Linux 环境配置
#    TargetConfig = $$PWD/config.ini
#    TargetEmoji = $$PWD/emoji.json
#    OutputDir = $$PWD/bin

#    # 输出调试信息
#    message("TargetConfig: $$TargetConfig")
#    message("TargetEmoji: $$TargetEmoji")
#    message("OutputDir: $$OutputDir")

#    # 拷贝 config.ini 和 emoji.json 到输出目录
#    QMAKE_POST_LINK += mkdir -p $$OutputDir
#    QMAKE_POST_LINK += cp -f "$$TargetConfig" "$$OutputDir"
#    QMAKE_POST_LINK += cp -f "$$TargetEmoji" "$$OutputDir"

#    LIBS += -L$$PWD/../third/linux/qtkeychain/lib -lqt5keychain
#}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../resources/pic.qrc \
    ../resources/qss.qrc \
