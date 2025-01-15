#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      主界面类 Function
 *
 * @author     hwk
 * @date       2024/12/27
 * @history
 *****************************************************************************/
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ChatDialog;
class LoginGUI;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(LoginGUI* login_gui, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ChatDialog * _chat_dlg;
    LoginGUI* _login_gui;
};
#endif // MAINWINDOW_H
