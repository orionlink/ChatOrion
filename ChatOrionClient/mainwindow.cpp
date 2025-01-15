#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chat_dialog.h"
#include "login_gui.h"

MainWindow::MainWindow(LoginGUI* login_gui, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _login_gui(login_gui)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/icon/ChatOrion.ico")); // 文件系统路径

    _chat_dlg = new ChatDialog();
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
//    this->setMinimumSize(QSize(1050,900));
//    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

MainWindow::~MainWindow()
{
    delete ui;
}
