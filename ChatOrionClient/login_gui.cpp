#include "login_gui.h"
#include "ui_login_gui.h"
#include "global.h"
#include "tools.h"
#include "network/http_mgr.h"
#include "tcp_mgr.h"
#include "user_data.h"
#include "user_mgr.h"
#include "tools.h"
#include "message_bus.h"

#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include <QIntValidator>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>
#include <QStyleOption>
#include <QJsonDocument>
#include <QJsonArray>

using namespace std;

LoginGUI::LoginGUI(QDialog*parent) :
    QDialog(parent),
    ui(new Ui::LoginGUI)
{
    ui->setupUi(this);
    ui->label_user_name->setScaledContents(true);   //图片自适应label大小
    ui->label_pwd->setScaledContents(true);         //图片自适应label大小

    setAttribute(Qt::WA_TranslucentBackground);  //设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);  //去掉窗口边框

    ui->lineE_pwd->setEchoMode(QLineEdit::Password);//设置为小黑点

    ui->frame_login->setCurrentIndex(0);

//    connect(ui->btn_1,SIGNAL(clicked(bool)),this,SLOT(set_style()));
//    connect(ui->btn_2,SIGNAL(clicked(bool)),this,SLOT(set_style()));
//    connect(ui->btn_3,SIGNAL(clicked(bool)),this,SLOT(set_style()));
//    connect(ui->btn_4,SIGNAL(clicked(bool)),this,SLOT(set_style()));

    QObject::connect(ui->btn_register, &QPushButton::clicked, this, &LoginGUI::registerPage);
    QObject::connect(ui->btn_register_2, &QPushButton::clicked, this, &LoginGUI::registerReq);
    QObject::connect(ui->btn_username, &QPushButton::clicked, this, &LoginGUI::loginPage);
    QObject::connect(ui->btn_login_page, &QPushButton::clicked, this, &LoginGUI::loginPage);
    QObject::connect(ui->btn_login, &QPushButton::clicked, this, &LoginGUI::login);
    QObject::connect(ui->btn_forget, &QPushButton::clicked, this, &LoginGUI::forgetPage);
    QObject::connect(ui->btn_direct_login, &QPushButton::clicked, this, &LoginGUI::loginPage);
    QObject::connect(ui->btn_submit, &QPushButton::clicked, this, &LoginGUI::forgetPassword);
    QObject::connect(ui->getcodeButton, &QPushButton::clicked, this, &LoginGUI::getCode);
    QObject::connect(ui->getcodeButton_2, &QPushButton::clicked, this, &LoginGUI::getCode);
    QObject::connect(ui->getcodeButton_3, &QPushButton::clicked, this, &LoginGUI::getCode);
    QObject::connect(ui->btn_email_login, &QPushButton::clicked, this, &LoginGUI::emailLogin);
    QObject::connect(ui->btn_wb, &QPushButton::clicked, this, &LoginGUI::emailPage);
    QObject::connect(ui->btn_wb_2, &QPushButton::clicked, this, &LoginGUI::emailPage);
    QObject::connect(ui->btn_wb_3, &QPushButton::clicked, this, &LoginGUI::emailPage);

    HttpMgr::GetInstance()->registerModulesHandlers(Modules::REGISTER_MOD, std::bind(&LoginGUI::regModCallback, this, std::placeholders::_1,
                                                                                     std::placeholders::_2, std::placeholders::_3));

    HttpMgr::GetInstance()->registerModulesHandlers(Modules::LOGIN_MOD, std::bind(&LoginGUI::loginModCallback, this, std::placeholders::_1,
                                                                                     std::placeholders::_2, std::placeholders::_3));

    //连接tcp连接请求的信号和槽函数
    connect(this, &LoginGUI::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    //连接tcp管理者发出的连接成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success, this, &LoginGUI::slot_tcp_con_finish);

    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_CHAT_LOGIN_RSP, std::bind(&LoginGUI::onChatLoginRsp, this, std::placeholders::_1,
                                                                                           std::placeholders::_2));

    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_LOGIN_LOAD_UNREAD_CHAT_MSG,
                                                   std::bind(&LoginGUI::LoginLoadUnRedChatMsg, this,
                                                   std::placeholders::_1, std::placeholders::_2));

    // 检查是否使用自动登录
    bool autoLogin = false;
    QString savedUsername = "";
    bool remember = false;
    Tools::loadLoginState(savedUsername, remember, autoLogin);
    ui->remPwd_checkBox->setChecked(remember);
    ui->autoLogin_checkBox->setChecked(autoLogin);

    if (remember || autoLogin)
    {
        Tools::loadPassword(savedUsername, this, [this, savedUsername, autoLogin](const QString& password)
        {
            if (!password.isEmpty())
            {
                qDebug() << "savedUsername: " << savedUsername << " password: " << password;
                // 自动填充并登录
                ui->lineE_user_name->setText(savedUsername);
                ui->lineE_pwd->setText(password);
                if (autoLogin) attemptAutoLogin(savedUsername, password);
            }
        });
    }

    QPushButton* btn_login = ui->btn_login;

    // 创建阴影效果并设置给按钮
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(2, 2);
    btn_login->setGraphicsEffect(shadowEffect);

    // 连接按钮的信号到槽来改变阴影效果
    connect(btn_login, &QPushButton::pressed, [=]() {
        shadowEffect->setOffset(1, 1);
        shadowEffect->setBlurRadius(5);
    });

    connect(btn_login, &QPushButton::released, [=]() {
        shadowEffect->setOffset(2, 2);
        shadowEffect->setBlurRadius(10);
    });

    // 设置为默认按钮，这样按下回车键时，如果当前焦点所在的控件不处理回车事件，则点击此按钮
    ui->btn_login->setDefault(true);

    // 设置自动默认，表示按钮能自动接收回车键作为触发
    ui->btn_login->setAutoDefault(true);

    ui->frame_err->hide();
    ui->frame_err_2->hide();
    ui->frame_err_3->hide();
    ui->frame_err_4->hide();

    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, this, &LoginGUI::updateButton);
    _remaining_time = 60; // 倒计时时间为60秒

    /// 邮箱格式
    QRegExp exp("[a-zA-Z0-9-_]+@[a-zA-Z0-9-_]+\\.[a-zA-Z]+");
    QRegExpValidator* rval = new QRegExpValidator(exp);
    ui->email_edit->setValidator(rval);
    ui->email_edit_2->setValidator(rval);
    ui->email_edit_3->setValidator(rval);

    ui->err_msg->setProperty("state","normal");
    repolish(ui->err_msg);

    ui->err_msg_2->setProperty("state","normal");
    repolish(ui->err_msg_2);

    ui->err_msg_3->setProperty("state","normal");
    repolish(ui->err_msg_3);

    ui->err_msg_4->setProperty("state","normal");
    repolish(ui->err_msg_4);

    ui->pass_visible->SetState("unvisible","unvisible_hover","","visible",
                                "visible_hover","");
    ui->pass_visible_2->SetState("unvisible","unvisible_hover","","visible",
                                "visible_hover","");
    ui->pass_visible_3->SetState("unvisible","unvisible_hover","","visible",
                                "visible_hover","");
    connect(ui->pass_visible, &ClickedLabel::clicked, this, &LoginGUI::onPassVisible);
    connect(ui->pass_visible_2, &ClickedLabel::clicked, this, &LoginGUI::onPassVisible);
    connect(ui->pass_visible_3, &ClickedLabel::clicked, this, &LoginGUI::onPassVisible);
}

void LoginGUI::login()
{
    if (!checkUserValid() || !checkPassValid())
    {
        return;
    }

    ui->btn_login->setEnabled(false);
    ui->btn_login->setText(QString::fromLocal8Bit("登陆中..."));

    QString username = ui->lineE_user_name->text().toUtf8().constData();
    QString password = Tools::hashString(ui->lineE_pwd->text(), QCryptographicHash::Sha256);

    QJsonObject request_json;
    request_json["username"] = username;
    request_json["password"] = password;
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_login"), request_json, Modules::LOGIN_MOD, ReqId::ID_LOGIN_USER);

    bool rem = ui->remPwd_checkBox->isChecked();
    bool autoLogin = ui->autoLogin_checkBox->isChecked();
    Tools::saveLoginState(username, rem, autoLogin);
    if (rem || autoLogin)
    {
        Tools::savePassword(username, ui->lineE_pwd->text());
    }
}

void LoginGUI::attemptAutoLogin(const QString &username, const QString &password)
{
    ui->btn_login->setEnabled(false);
    ui->btn_login->setText(QString::fromLocal8Bit("登陆中..."));

    QJsonObject request_json;
    request_json["username"] = username;
    request_json["password"] = Tools::hashString(password, QCryptographicHash::Sha256);
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_login"), request_json, Modules::LOGIN_MOD, ReqId::ID_LOGIN_USER);

    bool rem = ui->remPwd_checkBox->isChecked();
    Tools::saveLoginState(username, rem, false);
}

void LoginGUI::emailLogin()
{
    if (!checkEmailValid())
    {
        return;
    }

    if (!checkVarifyValid())
    {
        return;
    }

    ui->btn_email_login->setText(QString::fromLocal8Bit("正在登陆..."));
    ui->btn_email_login->setEnabled(false);

    QString email = ui->email_edit_2->text().toUtf8().constData();
    QString code = ui->code_edit_2->text().toUtf8().constData();

    QJsonObject request_json;
    request_json["email"] = email;
    request_json["code"] = code;
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/email_login"), request_json, Modules::LOGIN_MOD, ReqId::ID_LOGIN_EMAIL);
}

void LoginGUI::registerReq()
{
    if (!checkUserValid() || !checkPassValid() || !checkVarifyValid() || !checkEmailValid())
    {
        return;
    }

    ui->btn_register_2->setText(QString::fromLocal8Bit("正在注册..."));
    ui->btn_register_2->setEnabled(false);

    string username = ui->user_name_edit->text().toUtf8().constData();
    string password = Tools::hashString(ui->pwd_edit->text(), QCryptographicHash::Sha256).toStdString();
    string email = ui->email_edit->text().toUtf8().constData();
    string code = ui->code_edit->text().toUtf8().constData();

    QJsonObject request_json;
    request_json["username"] = username.c_str();
    request_json["password"] = password.c_str();
    request_json["email"] = email.c_str();
    request_json["code"] = code.c_str();
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_register"), request_json, Modules::REGISTER_MOD, ReqId::ID_REG_USER);
}

void LoginGUI::getCode()
{
    if (!checkEmailValid())
    {
        return;
    }

    int index = ui->frame_login->currentIndex();
    string email = "";
    switch (index)
    {
    case 1: /// 邮箱登陆
        email = ui->email_edit_2->text().toStdString();
        break;
    case 2: /// 注册
        email = ui->email_edit->text().toStdString();
        break;
    case 3: /// 忘记密码
        email = ui->email_edit_3->text().toStdString();
        break;
    default:
        break;
    }       

    QJsonObject request_json;
    request_json["email"] = email.c_str();
    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/get_varifycode"), request_json, Modules::REGISTER_MOD, ReqId::ID_GET_VARIFY_CODE);

#if 0
    // 开始倒计时
    ui->getcodeButton_2->setDisabled(true);
    ui->getcodeButton_3->setDisabled(true);
    ui->getcodeButton->setDisabled(true);
    _countdown_timer->start(1000); // 每秒更新一次
    _remaining_time = 60; // 重置倒计时时间
    ui->getcodeButton->setText(QString::number(_remaining_time) + QString::fromLocal8Bit(" 秒后重试"));
    ui->getcodeButton_2->setText(QString::number(_remaining_time) + QString::fromLocal8Bit(" 秒后重试"));
    ui->getcodeButton_3->setText(QString::number(_remaining_time) + QString::fromLocal8Bit(" 秒后重试"));
#endif
}

void LoginGUI::forgetPassword()
{
    if (!checkUserValid() || !checkPassValid() || !checkVarifyValid() || !checkEmailValid())
    {
        return;
    }

    ui->btn_submit->setText(QString::fromLocal8Bit("正在提交..."));
    ui->btn_submit->setEnabled(false);

    string username = ui->user_name_edit_2->text().toUtf8().constData();
    QString password = ui->pwd_edit_2->text();
    string email = ui->email_edit_3->text().toUtf8().constData();
    string code = ui->code_edit_3->text().toUtf8().constData();

    //发送http重置用户请求
    QJsonObject json_obj;
    json_obj["username"] = username.c_str();
    json_obj["password"] = Tools::hashString(password, QCryptographicHash::Sha256);
    json_obj["email"] = email.c_str();
    json_obj["code"] = code.c_str();

    HttpMgr::GetInstance()->postHttpReq(QUrl(gate_url_prefix + "/reset_pwd"),
                                        json_obj, Modules::REGISTER_MOD, ReqId::ID_RESET_PWD);
}

void LoginGUI::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess)
    {
      showTip(ui->err_msg, QString::fromLocal8Bit("聊天服务连接成功，正在登录..."), true);
      showTip(ui->err_msg_3, QString::fromLocal8Bit("聊天服务连接成功，正在登录..."), true);
      QJsonObject jsonObj;
      jsonObj["uid"] = _uid;
      jsonObj["token"] = _token;

      QJsonDocument doc(jsonObj);
      QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

      //发送tcp请求给chat server
      emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonData);
    }
    else
    {
        showTip(ui->err_msg, QString::fromLocal8Bit("网络异常"), true);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("网络异常"), true);
    }
}

void LoginGUI::loginPage()
{
    ui->frame_err->hide();
    ui->frame_err_2->hide();
    ui->frame_err_3->hide();
    ui->frame_err_4->hide();
    ui->frame_login->setCurrentIndex(0);
}

void LoginGUI::registerPage()
{
    ui->frame_err->hide();
    ui->frame_err_2->hide();
    ui->frame_err_3->hide();
    ui->frame_err_4->hide();
    ui->frame_login->setCurrentIndex(2);
}

void LoginGUI::emailPage()
{
    ui->frame_err->hide();
    ui->frame_err_2->hide();
    ui->frame_err_3->hide();
    ui->frame_err_4->hide();
    ui->frame_login->setCurrentIndex(1);
}

void LoginGUI::forgetPage()
{
    ui->frame_err->hide();
    ui->frame_err_2->hide();
    ui->frame_err_3->hide();
    ui->frame_err_4->hide();
    ui->frame_login->setCurrentIndex(3);
}

void LoginGUI::updateButton()
{
    _remaining_time--;
    if (_remaining_time <= 0)
    {
        _countdown_timer->stop();
        ui->getcodeButton->setEnabled(true);
        ui->getcodeButton_2->setEnabled(true);
        ui->getcodeButton_3->setEnabled(true);
        ui->getcodeButton->setText(QString::fromLocal8Bit("获取验证码"));
        ui->getcodeButton_2->setText(QString::fromLocal8Bit("获取验证码"));
        ui->getcodeButton_3->setText(QString::fromLocal8Bit("获取验证码"));
    }
    else 
    {
        ui->getcodeButton->setText(QString::number(_remaining_time) + QString::fromLocal8Bit(" 秒后重试"));
        ui->getcodeButton_2->setText(QString::number(_remaining_time) + QString::fromLocal8Bit(" 秒后重试"));
        ui->getcodeButton_3->setText(QString::number(_remaining_time) + QString::fromLocal8Bit(" 秒后重试"));
    }
}

void LoginGUI::regModCallback(ReqId id, QJsonObject res, ErrorCodes err)
{
    ui->btn_register_2->setEnabled(true);
    ui->btn_register_2->setText("注册");
    ui->btn_submit->setEnabled(true);
    ui->btn_submit->setText("提交");

    if (err == ErrorCodes::ERR_NETWORK || err == ErrorCodes::ERR_JSON)
    {
        showTip(ui->err_msg_2, QString::fromLocal8Bit("网络连接失败"), false);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("网络连接失败"), false);
        showTip(ui->err_msg_4, QString::fromLocal8Bit("网络连接失败"), false);
        return;
    }

    switch (id)
    {
    case ReqId::ID_GET_VARIFY_CODE:
    {
        int error = res["error"].toInt();
        if(error != ErrorCodes::SUCCESS)
        {
            showTip(ui->err_msg_2, QString::fromLocal8Bit("参数错误"), false);
            showTip(ui->err_msg_3, QString::fromLocal8Bit("参数错误"), false);
            showTip(ui->err_msg_4, QString::fromLocal8Bit("参数错误"), false);
            return;
        }
        auto email = res["email"].toString();
        showTip(ui->err_msg_2, QString::fromLocal8Bit("验证码已发送到邮箱，注意查收"), true);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("验证码已发送到邮箱，注意查收"), true);
        showTip(ui->err_msg_4, QString::fromLocal8Bit("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    }
        break;
    case ReqId::ID_REG_USER:
    {
        int error = res["error"].toInt();
        if(error != ErrorCodes::SUCCESS)
        {
            QString error_msg = res["error_msg"].toString();
            showTip(ui->err_msg_2, error_msg, false);
            showTip(ui->err_msg_3, error_msg, false);
            showTip(ui->err_msg_4, error_msg, false);
            return;
        }

        auto username = res["username"].toString();
        showTip(ui->err_msg_2, QString::fromLocal8Bit("用户注册成功，返回登录界面登录"), true);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("用户注册成功，返回登录界面登录"), true);
        showTip(ui->err_msg_4, QString::fromLocal8Bit("用户注册成功，返回登录界面登录"), true);
        qDebug()<< "username is " << username ;
    }
        break;
    case ReqId::ID_RESET_PWD:
    {
        int error = res["error"].toInt();
        if(error != ErrorCodes::SUCCESS)
        {
            QString error_msg = res["error_msg"].toString();
            showTip(ui->err_msg_2, error_msg, false);
            showTip(ui->err_msg_3, error_msg, false);
            showTip(ui->err_msg_4, error_msg, false);
            return;
        }

        auto username = res["username"].toString();
        showTip(ui->err_msg_2, QString::fromLocal8Bit("密码修改成功，返回登录界面登录"), true);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("密码修改成功，返回登录界面登录"), true);
        showTip(ui->err_msg_4, QString::fromLocal8Bit("密码修改成功，返回登录界面登录"), true);
        qDebug()<< "username is " << username ;
    }
        break;
    default:
        break;
    }
}

void LoginGUI::loginModCallback(ReqId id, QJsonObject res, ErrorCodes err)
{
    ui->btn_login->setEnabled(true);
    ui->btn_login->setText("登录");
    ui->btn_email_login->setEnabled(true);
    ui->btn_email_login->setText("登录");

    if (err == ErrorCodes::ERR_NETWORK || err == ErrorCodes::ERR_JSON)
    {
        showTip(ui->err_msg_2, QString::fromLocal8Bit("网络连接失败"), false);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("网络连接失败"), false);
        showTip(ui->err_msg_4, QString::fromLocal8Bit("网络连接失败"), false);
        return;
    }

    switch (id)
    {
    case ReqId::ID_LOGIN_USER:
    case ReqId::ID_LOGIN_EMAIL:
    {
        int error = res["error"].toInt();
        if(error != ErrorCodes::SUCCESS)
        {
            QString error_msg = res["error_msg"].toString();
            showTip(ui->err_msg, error_msg, false);
            showTip(ui->err_msg_3, error_msg, false);
            return;
        }
        auto username = res["username"].toString();
        _uid = res["uid"].toInt();
        _token = res["token"].toString();
        auto host = res["host"].toString();
        auto port = res["port"].toInt();
        showTip(ui->err_msg, QString::fromLocal8Bit("正在连接聊天服务器"), true);
        showTip(ui->err_msg_3, QString::fromLocal8Bit("正在连接聊天服务器"), true);
        qDebug()<< "username is " << username;

        emit sig_connect_tcp(host, port);
    }
        break;
    default:
        break;
    }
}

void LoginGUI::onPassVisible()
{
    auto state = ui->pass_visible->GetCurState();
    if(state == ClickLbState::Normal)
    {
        ui->lineE_pwd->setEchoMode(QLineEdit::Password);
    }
    else
    {
        ui->lineE_pwd->setEchoMode(QLineEdit::Normal);
    }

    state = ui->pass_visible_2->GetCurState();
    if(state == ClickLbState::Normal)
    {
        ui->pwd_edit->setEchoMode(QLineEdit::Password);
    }
    else
    {
        ui->pwd_edit->setEchoMode(QLineEdit::Normal);
    }

    state = ui->pass_visible_3->GetCurState();
    if(state == ClickLbState::Normal)
    {
        ui->pwd_edit_2->setEchoMode(QLineEdit::Password);
    }
    else
    {
        ui->pwd_edit_2->setEchoMode(QLineEdit::Normal);
    }
}

void LoginGUI::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void LoginGUI::onChatLoginRsp(int len, QByteArray data)
{
     // 将QByteArray转换为QJsonDocument
     QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

     // 检查转换是否成功
     if(jsonDoc.isNull()){
        qDebug() << "Failed to create QJsonDocument.";
        return;
     }

    QJsonObject jsonObj = jsonDoc.object();
    qDebug()<< "data jsonobj is " << jsonObj ;

    if(!jsonObj.contains("error"))
    {
        int err = ErrorCodes::ERR_JSON;
        qDebug() << "Login Failed, err is Json Parse Err" << err ;
        return;
    }

    int err = jsonObj["error"].toInt();

    QString result = QString("登录失败, err is %1")
                               .arg(err);

    if(err != ErrorCodes::SUCCESS)
    {
        QString error_msg = jsonObj["error_msg"].toString();
        showTip(ui->err_msg, result, false);
        showTip(ui->err_msg_3, result, false);
        qDebug() << "Login Failed, err is " << error_msg ;
        return;
    }

    auto uid = jsonObj["uid"].toInt();
    auto name = jsonObj["name"].toString();
    auto nick = jsonObj["nick"].toString();
    auto icon = jsonObj["icon"].toString();
    auto sex = jsonObj["sex"].toInt();
    auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);

    UserMgr::GetInstance()->SetUserInfo(user_info);
    UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());

    if(jsonObj.contains("apply_list")){
        UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
    }

   //添加好友列表
    if (jsonObj.contains("friend_list")) {
        UserMgr::GetInstance()->AppendFriendList(jsonObj["friend_list"].toArray());
    }

    QDialog::accept();
}

void LoginGUI::LoginLoadUnRedChatMsg(int len, QByteArray data)
{
    Q_UNUSED(len);
    // 将QByteArray转换为QJsonDocument
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    // 检查转换是否成功
    if (jsonDoc.isNull()) {
        qDebug() << "Failed to create QJsonDocument.";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("error"))
    {
        int err = ErrorCodes::ERR_JSON;
        qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
        return;
    }

    int err = jsonObj["error"].toInt();
    if (err != ErrorCodes::SUCCESS)
    {
        qDebug() << "Notify Chat Msg Failed, err is " << err;
        return;
    }

    qDebug() << "Receive Text Chat Notify Success " ;
    QJsonArray msgArray = jsonObj["messages"].toArray();
    for (int i = 0; i < msgArray.size(); ++i)
    {
        QJsonObject msg = msgArray.at(i).toObject();

        auto fromuid = msg["fromuid"].toInt();
        auto touid = msg["touid"].toInt();
        auto content = msg["content"].toString();
        auto msgid = msg["msgid"].toString();
        auto msg_type = msg["msg_type"].toInt();
        auto send_time = msg["send_time"].toInt();
        auto status = msg["status"].toInt();

        auto msg_ptr = std::make_shared<TextChatMsg>(fromuid,
                        touid, msgid, content);

        UserMgr::GetInstance()->AppendFriendChatMsg(fromuid, msg_ptr->_chat_msgs);
        UserMgr::GetInstance()->AppendSlefChatMsg(msg_ptr->_chat_msgs);
    }
}

void LoginGUI::showTip(QLabel *label, const QString &tip, bool is_ok)
{
    if (label)
    {
        ui->frame_err->show();
        ui->frame_err_2->show();
        ui->frame_err_3->show();
        ui->frame_err_4->show();

        qDebug() << "tip: " << tip;

        QPixmap pix;
        if (is_ok)
        {
            label->setProperty("state", "normal");

            pix.load(":/res/pic/icon_success_tip.png");
        }
        else
        {
            label->setProperty("state", "err");

            pix.load(":/res/pic/icon_error_tip.png");
        }

        ui->err_logo->setPixmap(pix);
        ui->err_logo_2->setPixmap(pix);
        ui->err_logo_3->setPixmap(pix);
        ui->err_logo_4->setPixmap(pix);

        label->setText(tip);
        repolish(label);
    }
}

bool LoginGUI::checkUserValid()
{
    if (ui->lineE_user_name->text().isEmpty() && ui->user_name_edit->text().isEmpty()
            && ui->user_name_edit_2->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_USER_ERR, "用户名不能为空");
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool LoginGUI::checkPassValid()
{
    if (ui->lineE_pwd->text().isEmpty() && ui->pwd_edit->text().isEmpty()
            && ui->pwd_edit_2->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_PWD_ERR, "密码不能为空");
        return false;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool LoginGUI::checkEmailValid()
{
    if (ui->email_edit->text().isEmpty() && ui->email_edit_2->text().isEmpty() && ui->email_edit_3->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_EMAIL_ERR, "邮箱不能为空");
        return false;
    }

    const QValidator* v1 = ui->email_edit->validator();
    const QValidator* v2 = ui->email_edit_2->validator();
    const QValidator* v3 = ui->email_edit_3->validator();
    int pos = 0;
    QString qemail_1 = ui->email_edit->text();
    QString qemail_2 = ui->email_edit_2->text();
    QString qemail_3 = ui->email_edit_3->text();
    if (v1->validate(qemail_1, pos) != QValidator::Acceptable
        && v2->validate(qemail_2, pos) != QValidator::Acceptable
        && v3->validate(qemail_3, pos) != QValidator::Acceptable)
    {
        AddTipErr(TipErr::TIP_EMAIL_ERR, "邮箱格式不正确");
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool LoginGUI::checkVarifyValid()
{
    if (ui->code_edit->text().isEmpty() && ui->code_edit_2->text().isEmpty() && ui->code_edit_3->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_VARIFY_ERR, "验证码不能为空");
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void LoginGUI::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(ui->err_msg, tips, false);
    showTip(ui->err_msg_2, tips, false);
    showTip(ui->err_msg_3, tips, false);
    showTip(ui->err_msg_4, tips, false);
}

void LoginGUI::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty())
    {
      ui->err_msg->clear();
      ui->err_msg_2->clear();
      ui->err_msg_3->clear();
      ui->err_msg_4->clear();
      ui->frame_err->hide();
      ui->frame_err_2->hide();
      ui->frame_err_3->hide();
      ui->frame_err_4->hide();
      return;
    }

    showTip(ui->err_msg, _tip_errs.first(), false);
    showTip(ui->err_msg_2, _tip_errs.first(), false);
    showTip(ui->err_msg_3, _tip_errs.first(), false);
    showTip(ui->err_msg_4, _tip_errs.first(), false);
}

/* 
* 槽函数-皮肤设置
*/
QPushButton* btn;
void LoginGUI::set_style()
{
    btn = qobject_cast<QPushButton*>(sender());//获取发射信号的对象
    QString filePath;
    if("btn_1" == btn->objectName())        //粉色
    {
        filePath = ":/res/qss/style-1.qss";
    }else if("btn_2" == btn->objectName())  //黄蓝
    {
        filePath = ":/res/qss/style-2.qss";
    }else if("btn_3" == btn->objectName())  //浅紫
    {
        filePath = ":/res/qss/style-3.qss";
    }else if("btn_4" == btn->objectName())  //青绿
    {
        filePath = ":/res/qss/style-4.qss";
    }

    /*皮肤设置*/
    QFile file(filePath);/*QSS文件所在的路径*/
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    ui->frame_background->setStyleSheet("");
    ui->frame_background->update();
    ui->frame_background->setStyleSheet(stylesheet);
    ui->frame_background->update();
    file.close();
}

LoginGUI::~LoginGUI()
{
    delete ui;
}

static bool mouse_press = false;
static QPoint mouse_point;

void LoginGUI::mouseMoveEvent(QMouseEvent * ev)
{
    /// 没有按下，处理原事件
    if (!mouse_press)
    {
        QWidget::mouseMoveEvent(ev);
        return;
    }
    auto cur_pos = ev->globalPos();
    this->move(cur_pos - mouse_point);

}

void LoginGUI::mousePressEvent(QMouseEvent* ev)
{
    /// 鼠标左键按下，记录位置
    if (ev->button() == Qt::LeftButton)
    {
        mouse_press = true;
        mouse_point = ev->pos();
    }
}

void LoginGUI::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);
    mouse_press = false;
}

