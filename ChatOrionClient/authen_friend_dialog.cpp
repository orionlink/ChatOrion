#include "authen_friend_dialog.h"
#include "ui_authen_friend_dialog.h"
#include "user_mgr.h"
#include "tcp_mgr.h"
#include "frameless_wid_drag_mgr.h"
#include "friend_label.h"

#include <QScrollBar>
#include <QMouseEvent>
#include <QJsonObject>
#include <QJsonDocument>

//申请好友标签输入框最低长度
static const int MIN_APPLY_LABEL_ED_LEN = 40;

static const QString add_prefix = "添加标签 ";

static const int  tip_offset = 5;

AuthenFriendDialog::AuthenFriendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthenFriendDialog),
    _label_point(2,6)
{
    ui->setupUi(this);

    FramelessWidDragMgr::GetInstance()->registerWindow(this);

    this->setObjectName("AuthenFriendDialog");
    ui->lb_ed->setPlaceholderText("搜索、添加标签...");
    ui->back_ed->setPlaceholderText("备注");

    ui->selected_multiple_lb->setPixmap(QPixmap(":/res/pic/打钩.png"));

    ui->lb_ed->SetMaxLength(21);
    ui->lb_ed->move(5, 5);
    ui->lb_ed->setFixedHeight(20);
    ui->input_tip_wid->hide();

    _tip_cur_point = QPoint(5, 5);

    _tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
                             "父与子学Python","nodejs开发指南","go 语言开发指南",
                                "游戏伙伴","金融投资","微信读书","拼多多拼友" };

    InitTipLbs();

    //链接输入标签回车事件
    connect(ui->lb_ed, &CustomizeEdit::returnPressed, this, &AuthenFriendDialog::SlotLabelEnter);
    connect(ui->lb_ed, &CustomizeEdit::textChanged, this, &AuthenFriendDialog::SlotLabelTextChange);
    connect(ui->lb_ed, &CustomizeEdit::editingFinished, this, &AuthenFriendDialog::SlotLabelEditFinished);
    connect(ui->tip_lb, &ClickedOnceLabel::clicked, this, &AuthenFriendDialog::SlotAddFirendLabelByClickTip);

    connect(ui->cancel_btn, &QPushButton::clicked, this, &AuthenFriendDialog::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &AuthenFriendDialog::SlotApplySure);

    installEventFilter(this);
}

AuthenFriendDialog::~AuthenFriendDialog()
{
    qDebug()<< "AuthenFriendDialog destruct";
    delete ui;
}

void AuthenFriendDialog::InitTipLbs()
{
}

void AuthenFriendDialog::AddTipLbs(ClickedLabel *, QPoint cur_point, QPoint &next_point, int text_width, int text_height)
{

}

bool AuthenFriendDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        if (mouse_event)
        {
            QPoint posInChatOnly = ui->chat_only_wid->mapFromGlobal(mouse_event->globalPos());
            QPoint posInMultiple = ui->multiple_wid->mapFromGlobal(mouse_event->globalPos());

            if (ui->chat_only_wid->rect().contains(posInChatOnly))
            {
                ui->selected_chat_only_lb->setPixmap(QPixmap(":/res/pic/打钩.png"));
                ui->selected_multiple_lb->setPixmap(QPixmap(":/res/pic/close_transparent.png"));
                ui->selected_permi_wid->hide();
            }

            if (ui->multiple_wid->rect().contains(posInMultiple))
            {
                ui->selected_multiple_lb->setPixmap(QPixmap(":/res/pic/打钩.png"));
                ui->selected_chat_only_lb->setPixmap(QPixmap(":/res/pic/close_transparent.png"));
                ui->selected_permi_wid->show();
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void AuthenFriendDialog::SetApplyInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    _apply_info = apply_info;
    ui->back_ed->setPlaceholderText(apply_info->_name);
}

void AuthenFriendDialog::resetLabels()
{
    auto max_width = ui->grid_wid->width();
    auto label_height = 0;
    for(auto iter = _friend_labels.begin(); iter != _friend_labels.end(); iter++){
        //todo... 添加宽度统计
        if( _label_point.x() + iter.value()->width() > max_width) {
            _label_point.setY(_label_point.y()+iter.value()->height()+6);
            _label_point.setX(2);
        }

        iter.value()->move(_label_point);
        iter.value()->show();

        _label_point.setX(_label_point.x()+iter.value()->width()+2);
        _label_point.setY(_label_point.y());
        label_height = iter.value()->height();
    }

    if(_friend_labels.isEmpty()){
         ui->lb_ed->move(_label_point);
         return;
    }

    if(_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->grid_wid->width()){
        ui->lb_ed->move(2,_label_point.y()+label_height+6);
    }else{
         ui->lb_ed->move(_label_point);
    }
}

void AuthenFriendDialog::addLabel(QString name)
{
    if (_friend_labels.find(name) != _friend_labels.end()) {
        ui->lb_ed->clear();
        return;
    }

    auto tmplabel = new FriendLabel(ui->grid_wid);
    tmplabel->SetText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = ui->grid_wid->width();
    //todo... 添加宽度统计
    if (_label_point.x() + tmplabel->width() > max_width) {
        _label_point.setY(_label_point.y() + tmplabel->height() + 6);
        _label_point.setX(2);
    }
    else {

    }


    tmplabel->move(_label_point);
    tmplabel->show();
    _friend_labels[tmplabel->Text()] = tmplabel;
    _friend_label_keys.push_back(tmplabel->Text());

    connect(tmplabel, &FriendLabel::sig_close, this, &AuthenFriendDialog::SlotRemoveFriendLabel);

    _label_point.setX(_label_point.x() + tmplabel->width() + 2);

    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->grid_wid->width()) {
        ui->lb_ed->move(2, _label_point.y() + tmplabel->height() + 2);
    }
    else {
        ui->lb_ed->move(_label_point);
    }

    ui->lb_ed->clear();

    if (ui->grid_wid->height() < _label_point.y() + tmplabel->height() + 2) {
        ui->grid_wid->setFixedHeight(_label_point.y() + tmplabel->height() * 2 + 2);
    }
}

void AuthenFriendDialog::ShowMoreLabel()
{

}

void AuthenFriendDialog::SlotLabelEnter()
{
    if(ui->lb_ed->text().isEmpty()){
        return;
    }

    auto text = ui->lb_ed->text();

    addLabel(ui->lb_ed->text());

    ui->input_tip_wid->hide();

    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    //找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end())
    {
        _tip_data.push_back(text);
    }
}

void AuthenFriendDialog::SlotRemoveFriendLabel(QString name)
{
    _label_point.setX(2);
    _label_point.setY(6);

   auto find_iter = _friend_labels.find(name);

   if(find_iter == _friend_labels.end()){
       return;
   }

   auto find_key = _friend_label_keys.end();
   for(auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end();
       iter++){
       if(*iter == name){
           find_key = iter;
           break;
       }
   }

   if(find_key != _friend_label_keys.end()){
      _friend_label_keys.erase(find_key);
   }


   delete find_iter.value();

   _friend_labels.erase(find_iter);

   resetLabels();

   auto find_add = _add_labels.find(name);
   if(find_add == _add_labels.end()){
        return;
   }

   find_add.value()->ResetNormalState();
}

void AuthenFriendDialog::SlotChangeFriendLabelByTip(QString text, ClickLbState)
{
}

void AuthenFriendDialog::SlotLabelTextChange(const QString &text)
{
    if (text.isEmpty()) {
        ui->tip_lb->setText("");
        ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(_tip_data.begin(), _tip_data.end(), text);
    if (iter == _tip_data.end()) {
        auto new_text = add_prefix + text;
        ui->tip_lb->setText(new_text);
        ui->input_tip_wid->show();
        return;
    }
    ui->tip_lb->setText(text);
    ui->input_tip_wid->show();
}

void AuthenFriendDialog::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void AuthenFriendDialog::SlotAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1) {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);

    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    //找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end())
    {
        _tip_data.push_back(text);
    }
}

void AuthenFriendDialog::SlotApplySure()
{
    qDebug() << "Slot Apply Sure ";
    //添加发送逻辑
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->GetUid();
    jsonObj["fromuid"] = uid;
    jsonObj["touid"] = _apply_info->_uid;
    QString back_name = "";
    if(ui->back_ed->text().isEmpty()){
        back_name = ui->back_ed->placeholderText();
    }else{
        back_name = ui->back_ed->text();
    }
    jsonObj["back_name"] = back_name;
    jsonObj["apply_name"] = _apply_info->_name;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_AUTH_FRIEND_REQ, jsonData);

    this->hide();
    deleteLater();
}

void AuthenFriendDialog::SlotApplyCancel()
{
    this->hide();
    deleteLater();
}

