#include "mainwindow.h"
#include "login_gui.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/res/qss/style-2.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();

    a.setStyleSheet(stylesheet);
    a.setWindowIcon(QIcon(":/res/icon/ChatOrion.ico"));

    LoginGUI login_gui;
    if (login_gui.exec() == QDialog::Rejected)
    {
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
