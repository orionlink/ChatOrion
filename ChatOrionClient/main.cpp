#include "mainwindow.h"
#include "login_gui.h"
#include "global.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSettings>
#include <QDebug>

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

    MainWindow w(&login_gui);
    w.show();
    return a.exec();
}
