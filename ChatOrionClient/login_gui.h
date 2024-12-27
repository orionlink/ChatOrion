#ifndef LOGIN_GUI_H
#define LOGIN_GUI_H

#include <QWidget>
#include <QDialog>
/******************************************************************************
 *
 * @file       login_gui.h
 * @brief      登录窗口类 Function
 *
 * @author     hwk
 * @date       2024/12/27
 * @history
 *****************************************************************************/

namespace Ui {
class LoginGUI;
}

class LoginGUI : public QDialog
{
    Q_OBJECT

public:
    explicit LoginGUI(QDialog*parent = 0);
    ~LoginGUI();

    void mouseMoveEvent(QMouseEvent* ev)override;
    void mousePressEvent(QMouseEvent* ev)override;
    void mouseReleaseEvent(QMouseEvent* ev)override;

public slots:
    void set_style();

    void Login();

    void LoginPage();

    void RegisterPage();

    void EmailPage();

    void ForgetPage();

    void Register();

    void GetCode();

    void EmailLogin();

    void ForgetPassword();

    void UpdateButton(); // 新增的用于更新按钮状态的槽
protected:
    void paintEvent(QPaintEvent*) override;

private:
    Ui::LoginGUI*ui;

    QTimer* _countdown_timer; // 新增的计时器
    int _remaining_time;      // 倒计时剩余时间
};

#endif // LOGIN_GUI_H
