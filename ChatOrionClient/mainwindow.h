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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
