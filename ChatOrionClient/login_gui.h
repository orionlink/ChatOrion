#ifndef LOGIN_GUI_H
#define LOGIN_GUI_H

#include <QWidget>
#include <QDialog>
#include <QMap>
#include <QJsonObject>
#include <QMetaObject>
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
class QLabel;

#include "global.h"

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

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

    /**
     * @brief 登录请求
     */
    void login();

    /**
     * @brief 登录界面切换
     */
    void loginPage();

    /**
     * @brief 注册界面切换
     */
    void registerPage();

    /**
     * @brief 邮箱界面切换
     */
    void emailPage();

    /**
     * @brief 忘记密码界面切换
     */
    void forgetPage();

    /**
     * @brief 发起注册请求
     */
    void registerReq();

    /**
     * @brief 发起获取验证码请求
     */
    void getCode();

    /**
     * @brief 邮箱登录请求
     */
    void emailLogin();

    /**
     * @brief 忘记密码请求
     */
    void forgetPassword();

    void updateButton(); // 新增的用于更新按钮状态的槽

    void regModCallback(ReqId id, QJsonObject res, ErrorCodes err);

    void onPassVisible();
protected:
    void paintEvent(QPaintEvent*) override;
private:
    void showTip(QLabel* label, const QString& tip, bool is_ok);
    void initModulesHandlers();

    bool checkUserValid();
    bool checkPassValid();
    bool checkEmailValid();
    bool checkVarifyValid();

    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
private:
    Ui::LoginGUI*ui;

    QMap<TipErr, QString> _tip_errs;
    QTimer* _countdown_timer; // 新增的计时器
    int _remaining_time;      // 倒计时剩余时间
};

#endif // LOGIN_GUI_H
