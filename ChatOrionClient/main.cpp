#include "chat_dialog.h"
#include "login_gui.h"
#include "global.h"
#include "FrameWgt/framewgt.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSettings>
#include <QIcon>
#include <QDebug>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<ReqId>();
    qRegisterMetaType<ErrorCodes>();
    qRegisterMetaType<Modules>();


    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    // 拼接文件名
    QString fileName = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path +
                            QDir::separator() + fileName);

    qDebug() << "config_path: " << config_path;
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+ gate_port;
    qDebug() << "gate_url_prefix: " << gate_url_prefix;

    global_emoji_name = settings.value("EmojiName/name").toString();

    QFile file(":/res/qss/style-2.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();

    a.setStyleSheet(stylesheet);
    a.setWindowIcon(QIcon(":/res/icon/ChatOrion.ico"));

    LoginGUI login_gui;
//    if (login_gui.exec() == QDialog::Rejected)
//    {
//        return -1;
//    }


    ChatDialog* dlg = new ChatDialog();

    FrameWgt w(dlg);

    // 获取鼠标的全局位置
    QPoint mousePos = QCursor::pos();

    // 获取鼠标所在的屏幕
    QScreen *mouseScreen = QGuiApplication::screenAt(mousePos);
    if (!mouseScreen) {
        qWarning() << "No screen found at mouse position!";
        return -1;
    }

    // 获取鼠标所在屏幕的几何信息
    QRect screenGeometry = mouseScreen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    // 将窗口移动到鼠标所在屏幕的中心
    int x = screenGeometry.x() + (screenWidth - 800) / 2;
    int y = screenGeometry.y() + (screenHeight - 600) / 2;

    w.setBlurRadius(5);
    w.resize(800, 600);
    w.move(x, y);
    w.show();

    return a.exec();
}
