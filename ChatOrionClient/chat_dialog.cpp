#include "chat_dialog.h"
#include "ui_chat_dialog.h"
#include "chat_user_item.h"
#include "common_utils.h"
#include "cui_helper.h"
#include "message_bus.h"
#include "tcp_mgr.h"
#include "user_mgr.h"
#include "con_user_item.h"
#include "loading_dlg.h"
#include "self_info_dialog.h"

#include <QMenu>
#include <QFile>
#include <QTextStream>
#include <QAction>
#include <QRandomGenerator>

/************************测试使用　开始**************************/
inline std::vector<QString>  test_strs ={"你好",
                             "在干嘛",
                             "好累呀",
                            "You have to love yourself",
                            "今天有大新闻"};

inline std::vector<QString> heads = {
    ":/res/pic/head_1.jpg",
    ":/res/pic/head_2.jpg",
    ":/res/pic/head_3.jpg",
    ":/res/pic/head_4.jpg",
    ":/res/pic/head_5.jpg"
};

inline std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};

/************************测试使用 结束**************************/

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog),
    _current_chat_uid(0),
    _b_loading(false),
    _unread_msg_total_count(0)
{
    ui->setupUi(this);

    this->setWindowTitle(QStringLiteral("微信"));

    ui->side_chat_lb->setProperty("state","normal");

    ui->side_setting_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_chat_lb->setToolTip(QStringLiteral("聊天"));
    ui->side_contact_lb->setToolTip(QStringLiteral("联系人"));
    ui->side_collect_lb->setToolTip(QStringLiteral("收藏"));
    ui->side_circle_lb->setToolTip(QStringLiteral("朋友圈"));
    ui->side_watch_lb->setToolTip(QStringLiteral("看一看"));
    ui->side_search_lb->setToolTip(QStringLiteral("搜一搜"));
    ui->side_applet_lb->setToolTip(QStringLiteral("小程序面板"));
    ui->side_setting_lb->setToolTip(QStringLiteral("设置及其他"));

    // 默认选择聊天
    ui->side_chat_lb->SetSelected(true);

    // 显示搜索图标
    QAction* searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/pic/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    ui->search_edit->setMaxLength(15);

    // 添加清除按钮
    QAction* clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/pic/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    QObject::connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text)
    {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/res/pic/close_search.png"));
        } else {
            clearAction->setIcon(QIcon(":/res/pic/close_transparent.png")); // 文本为空时，切换回透明图标
        }

        MessageBus::instance()->sendMessage(MessageCommand::SEARCH_EDIT_TEXT_CHANGED, text);
    });

    QObject::connect(clearAction, &QAction::triggered, [this, clearAction]
    {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/res/pic/close_transparent.png")); // 清除文本后，切换回透明图标
        ui->search_edit->clearFocus();
    });

    addLBGroup(ui->side_chat_lb);
    addLBGroup(ui->side_contact_lb);
    addLBGroup(ui->side_collect_lb);

    connect(ui->side_head_lb, &ClickedLabel::clicked, this, &ChatDialog::slot_side_head);
    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    connect(ui->side_collect_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_collect);
    connect(ui->side_setting_lb, &ClickedLabel::clicked, this, &ChatDialog::slot_setting_label);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_search_edit_text_changed);

    //连接联系人页面点击好友申请条目的信号
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this,&ChatDialog::slot_switch_apply_friend_page);

    //连接加载联系人的信号和槽函数
    connect(ui->con_user_list, &ContactUserList::sig_loading_contact_user,
            this, &ChatDialog::slot_loading_contact_user);

    // 加载更多的聊天用户
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user,
           this, &ChatDialog::slot_loading_chat_user);

    //连接searchlist跳转聊天信号
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);

    //连接好友信息界面发送的点击事件
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this,
            &ChatDialog::slot_jump_chat_item_from_infopage);

    //连接聊天列表点击信号
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_chat_user_item_clicked);

    //连接点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->con_user_list, &ContactUserList::sig_switch_friend_info_page,
            this,&ChatDialog::slot_friend_info_page);

    // 客户端接收chat_page缓存消息
    connect(ui->chat_page, &ChatPage::sig_append_send_chat_msg, this, &ChatDialog::slot_append_send_chat_msg);

    connect(ui->apply_friend_page, &ApplyFriendPage::sig_presence_apply, this, &ChatDialog::slot_presence_apply);

    connect(this, &ChatDialog::sig_notify_text_chat_msg, ui->chat_page, &ChatPage::slot_notify_text_chat_msg);

    //链接自己认证回复信号
    MessageBus::instance()->registerHandler(MessageCommand::AUTH_FRIEND_RSP, this, &ChatDialog::onAuthFriendRsp);
     //连接认证添加好友信号
    MessageBus::instance()->registerHandler(MessageCommand::NOTIFY_AUTH_FRIEND_REQ, this, &ChatDialog::onNotifyAuthFriendReq);

    ui->user_stacked->setCurrentWidget(ui->chat_user_list_page);
    ui->stackedWidget->setCurrentWidget(ui->normal_page);

    CommonUtils::loadStyleSheet(this, "chatWin");

    installEventFilter(this);

    // 注册网络处理函数
    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_NOTIFY_ADD_FRIEND_REQ,
                                                   std::bind(&ChatDialog::NotifyAddFriendReq, this,
                                                   std::placeholders::_1, std::placeholders::_2));

    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_NOTIFY_TEXT_CHAT_MSG_REQ,
                                                   std::bind(&ChatDialog::NotifyTextChatMsgReq, this,
                                                   std::placeholders::_1, std::placeholders::_2));

    // 初始化聊天用户；列表
//    addChatUserList();

    MessageBus::instance()->registerHandler(MessageCommand::LOGIN_LOAD_UNREAD_CHAT_MSG, this, [this](const QVariant& data)
    {
        bool is_load = data.toBool();
        if (is_load)
        {
            loadMoreChatUser();
            loadMoreConUser();
        }
    });


    // 以后从网络加载头像
    QPixmap pixmap(":/res/pic/head_5.jpg");
    ui->side_head_lb->setPixmap(pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->side_head_lb->setScaledContents(true);

#if 0
    ui->side_contact_lb->SetRedDot(true);
    ui->side_chat_lb->SetRedDot(true);
    ui->con_user_list->SetRedDot(true);
#endif
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::addChatUserList()
{
    //先按照好友列表加载聊天记录，等以后客户端实现聊天记录数据库之后再按照最后信息排序
    loadMoreChatUser();

#if 0
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 50; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % test_strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();

        int msg_count = QRandomGenerator::global()->bounded(110);

        bool is_show = QRandomGenerator::global()->bounded(2);
        auto *chat_user_wid = new ChatUserItem();
        chat_user_wid->SetRedDot(is_show, msg_count);
        chat_user_wid->SetInfo(names[name_i], heads[head_i], test_strs[str_i]);
        QListWidgetItem *item = new QListWidgetItem();
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
#endif
}

void ChatDialog::loadMoreChatUser()
{
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if (friend_list.empty()) return;


    for(auto & friend_ele : friend_list)
    {
        auto find_iter = _chat_items_added.find(friend_ele->_uid);
        if(find_iter != _chat_items_added.end())
        {
            continue;
        }

        auto *chat_user_wid = new ChatUserItem();
        auto user_info = std::make_shared<UserInfo>(friend_ele);
#if 1
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int head_i = randomValue % heads.size();
        user_info->_icon = heads[head_i];
#endif
        chat_user_wid->SetInfo(user_info);
        _chat_user_unread_msg_count.insert(user_info->_uid, 0);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
        _chat_items_added.insert(friend_ele->_uid, item);
    }

    ui->chat_user_list->update();

    //更新已加载条目
    UserMgr::GetInstance()->UpdateChatLoadedCount();
}

void ChatDialog::loadMoreConUser()
{
    auto friend_list = UserMgr::GetInstance()->GetConListPerPage();
    if (friend_list.empty()) return;

    for(auto & friend_ele : friend_list)
    {
        auto *chat_user_wid = new ConUserItem();

#if 1
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int head_i = randomValue % heads.size();
        friend_ele->_icon = heads[head_i];
#endif
        chat_user_wid->SetInfo(friend_ele->_uid,friend_ele->_name,
                               friend_ele->_icon);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->con_user_list->addItem(item);
        ui->con_user_list->setItemWidget(item, chat_user_wid);
    }

    //更新已加载条目
    UserMgr::GetInstance()->UpdateContactLoadedCount();
}

void ChatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgdata)
{
    for(auto & msg : msgdata)
    {
        if(msg->_from_uid != _current_chat_uid)
        {
            break;
        }

        ui->chat_page->AppendChatMsg(msg);
    }
}

void ChatDialog::SetSelectChatItem(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }

    if(uid == 0){
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem *firstItem = ui->chat_user_list->item(0);
        if(!firstItem){
            return;
        }

        //转为widget
        QWidget *widget = ui->chat_user_list->itemWidget(firstItem);
        if(!widget){
            return;
        }

        auto con_item = qobject_cast<ChatUserItem*>(widget);
        if(!con_item){
            return;
        }

        _current_chat_uid = con_item->GetUserInfo()->_uid;

        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        qDebug() << "uid " <<uid<< " not found, set curent row 0";
        ui->chat_user_list->setCurrentRow(0);
        return;
    }

    ui->chat_user_list->setCurrentItem(find_iter.value());

    _current_chat_uid = uid;
}

void ChatDialog::addLBGroup(StateWidget *lb)
{
    _side_lb_list.push_back(lb);
}

void ChatDialog::clearLabelState(StateWidget *lb)
{
    for(auto & ele: _side_lb_list){
        if(ele == lb){
            continue;
        }

        ele->ClearState();
    }
}

void ChatDialog::NotifyAddFriendReq(int len, QByteArray data)
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
        qDebug() << "Login Failed, err is Json Parse Err" << err;
        return;
    }

    int err = jsonObj["error"].toInt();
    if (err != ErrorCodes::SUCCESS)
    {
        qDebug() << "Login Failed, err is " << err;
        return;
    }

     int from_uid = jsonObj["applyuid"].toInt();
     QString applyname = jsonObj["name"].toString();
     QString desc = jsonObj["desc"].toString();
     QString icon = jsonObj["icon"].toString();
     QString nick = jsonObj["nick"].toString();
     int sex = jsonObj["sex"].toInt();

    auto apply_info = std::make_shared<AddFriendApply>(
                from_uid, applyname, desc,
                  icon, nick, sex);

    bool b_already = UserMgr::GetInstance()->AlreadyApply(apply_info->_from_uid);
    if(b_already){
         return;
    }

    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply_info));

    ui->apply_friend_page->AddNewApply(apply_info);

    if (!ui->side_contact_lb->isSelected())
    {
        ui->side_contact_lb->SetRedDot(true);
    }

    ui->con_user_list->SetRedDot(true);
}

void ChatDialog::NotifyTextChatMsgReq(int len, QByteArray data)
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

    // 每次接收到消息，未读消息++
    _unread_msg_total_count++;
    if (!ui->side_chat_lb->isSelected())
    {
        ui->side_chat_lb->SetRedDot(true, _unread_msg_total_count);
    }

    auto fromuid = jsonObj["from_uid"].toInt();
    auto touid = jsonObj["to_uid"].toInt();
    auto content = jsonObj["content"].toString();
    auto msgid = jsonObj["msg_id"].toString();
    auto send_time = jsonObj["send_time"].toInt();
    auto msg_type = jsonObj["msg_type"].toInt();

    auto msg_ptr = std::make_shared<TextChatMsg>(fromuid,
                    touid, msgid, content, send_time, msg_type, 0);

    ChatUserItem* chat_user_item = nullptr;

    auto find_iter = _chat_items_added.find(fromuid);
    if(find_iter != _chat_items_added.end())
    {
        qDebug() << "set chat item msg, uid is " <<fromuid;

        QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
        chat_user_item = qobject_cast<ChatUserItem*>(widget);
    }
    else
    {
        //如果没找到，则创建新的插入listwidget
        chat_user_item = new ChatUserItem();
        //查询好友信息
        auto fi_ptr = UserMgr::GetInstance()->GetFriendById(fromuid);
        chat_user_item->SetInfo(fi_ptr);

        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(chat_user_item->sizeHint());

        ui->chat_user_list->insertItem(0, item);
        ui->chat_user_list->setItemWidget(item, chat_user_item);

        _chat_items_added.insert(fromuid, item);
    }

    if (chat_user_item == nullptr) return;

    chat_user_item->updateLastMsg(msg_ptr->_chat_msgs);

    int unread_count = _chat_user_unread_msg_count.value(fromuid);
    unread_count++;
    _chat_user_unread_msg_count[fromuid] = unread_count;
    if (!chat_user_item->isSelected())
    {
        chat_user_item->SetRedDot(true, unread_count);
    }

    // 如果当前ui->side_chat_lb是选择状态，并且chat_user_item也是选中状态，则总计数器需要减去
    if (ui->side_chat_lb->isSelected() && chat_user_item->isSelected() && _unread_msg_total_count > 0)
    {
        _unread_msg_total_count--;
    }

    //更新当前聊天页面记录
    UpdateChatMsg(msg_ptr->_chat_msgs);
    UserMgr::GetInstance()->AppendFriendChatMsg(fromuid, msg_ptr->_chat_msgs);

    emit sig_notify_text_chat_msg(msg_ptr->_chat_msgs);
}

bool ChatDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        if (mouse_event)
        {
            if (ui->user_stacked->currentWidget() == ui->search_list_page)
            {
                // 将鼠标点击位置转换为搜索列表坐标系中的位置
                QPoint posInSearchList = ui->search_list->mapFromGlobal(mouse_event->globalPos());
                // 判断点击位置是否在聊天列表的范围内
                if (!ui->search_list->rect().contains(posInSearchList))
                {
                    ui->search_edit->clear(); // 只需清空即可，会触发slot_search_edit_text_changed函数实现切换逻辑
                }
            }

            QPoint posInDialog = mouse_event->globalPos();

            // 判断点击位置是否在 QDialog 的范围内
            auto find_dlg = ui->search_list->getFindSuccessDialog();
            if (find_dlg)
            {
                if (!find_dlg->geometry().contains(posInDialog)) {
                    ui->search_list->CloseFindDlg();
                    qDebug() << "Dialog closed because click was outside.";
                }
            }

            auto find_failed_dlg = ui->search_list->getFindFailedDialog();
            if (find_failed_dlg)
            {
                if (!find_failed_dlg->geometry().contains(posInDialog)) {
                    ui->search_list->CloseFindDlg();
                    qDebug() << "Dialog closed because click was outside.";
                }
            }

            if (_selfInfoDialog)
            {
                if (!_selfInfoDialog->geometry().contains(posInDialog)) {
                    _selfInfoDialog->setVisible(false);
                    qDebug() << "Dialog closed because click was outside.";
                }
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}

void ChatDialog::slot_side_chat()
{
    ui->side_chat_lb->SetRedDot(false);
    clearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    ui->user_stacked->setCurrentWidget(ui->chat_user_list_page);

    QWidget *widget = ui->chat_user_list->itemWidget(ui->chat_user_list->currentItem());
    if(!widget)
    {
        ui->stackedWidget->setCurrentWidget(ui->normal_page);
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ChatUserItem *chatItem = qobject_cast<ChatUserItem*>(widget);
    if (chatItem && _unread_msg_total_count > 0)
    {
        int uid = chatItem->GetUserInfo()->_uid;

        _unread_msg_total_count -= _chat_user_unread_msg_count.value(uid);
        if (_unread_msg_total_count < 0) _unread_msg_total_count = 0;

        _chat_user_unread_msg_count[uid] = 0;
    }
}

void ChatDialog::slot_side_contact()
{
    ui->side_contact_lb->SetRedDot(false);
    clearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->normal_page);
    ui->user_stacked->setCurrentWidget(ui->con_user_list_page);
}

void ChatDialog::slot_side_collect()
{
    clearLabelState(ui->side_collect_lb);
    ui->stackedWidget->setCurrentWidget(ui->normal_page);
    ui->user_stacked->setCurrentWidget(ui->collect_list_page);
}

void ChatDialog::slot_setting_label()
{
    QMenu * menu = new QMenu(this);
    CUIHelper::GetInstance()->setMenuRadius(menu);

    QAction* setupAction = new QAction(menu);
    setupAction->setText(QStringLiteral("设置"));
    menu->addAction(setupAction);

    QAction* feedbackAction = new QAction(menu);
    feedbackAction->setText(QStringLiteral("意见反馈"));
    menu->addAction(feedbackAction);

    QAction* lockAction = new QAction(menu);
    lockAction->setText(QStringLiteral("锁定"));
    menu->addAction(lockAction);

    // 获取控件的全局位置
    QPoint globalPos = ui->side_setting_lb->mapToGlobal(QPoint(0, 0));

    // 调整菜单的位置，使其显示在控件的右侧
    globalPos.setX(globalPos.x() + ui->side_setting_lb->width());
    globalPos.setY(globalPos.y());

    menu->exec(globalPos);
}

void ChatDialog::slot_side_head()
{
    if (!_selfInfoDialog) {
        _selfInfoDialog = new SelfInfoDialog(this);
        auto user_info = UserMgr::GetInstance()->GetUserInfo();
        auto self = qobject_cast<SelfInfoDialog*>(_selfInfoDialog);
        self->SetUserInfo(user_info);
    }

    QPoint headGlobalPos = ui->side_head_lb->mapToGlobal(QPoint(0, 0));

    QSize btnSize = ui->side_head_lb->size();

    // 计算窗口位置
    // x: 按钮的右边界
    // y: 按钮的底部位置
    QPoint dialogPoint(
        headGlobalPos.x() + btnSize.width(),  // 右对齐
        headGlobalPos.y() + btnSize.height()  // 在按钮下方
    );

    _selfInfoDialog->move(dialogPoint);

    _selfInfoDialog->setVisible(!_selfInfoDialog->isVisible());
}

void ChatDialog::slot_presence_apply(int new_apply_count)
{
    if (new_apply_count == 0) return;

    if (!ui->side_contact_lb->isSelected())
    {
        ui->side_contact_lb->SetRedDot(true, new_apply_count);
    }

    ui->con_user_list->SetRedDot(true);
}

void ChatDialog::slot_search_edit_text_changed()
{
    // 搜索页面索引为0， 消息页面为1，联系人为2
    static int user_stacked_last_index = 0;
    if (!ui->search_edit->text().isEmpty())
    {
        if (user_stacked_last_index == 0) user_stacked_last_index = ui->user_stacked->currentIndex();

        ui->user_stacked->setCurrentWidget(ui->search_list_page);
    }
    else
    {
        ui->user_stacked->setCurrentIndex(user_stacked_last_index);
        user_stacked_last_index = 0;
    }
}

void ChatDialog::slot_switch_apply_friend_page()
{
    ui->stackedWidget->setCurrentWidget(ui->apply_friend_page);
}

void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si)
{
    auto user_info = std::make_shared<UserInfo>(si);

    if (user_info->_chat_msgs.empty())
    {
        auto userInfo = UserMgr::GetInstance()->GetUserInfo();
        user_info->_chat_msgs.clear();
        if (userInfo)
        {
            for (auto msg : userInfo->_chat_msgs)
            {
                if (msg->_to_uid == user_info->_uid || user_info->_uid == msg->_from_uid)
                {
                    user_info->_chat_msgs.push_back(msg);
                }
            }
        }
    }

    auto find_iter = _chat_items_added.find(si->_uid);
    if(find_iter != _chat_items_added.end())
    {
        qDebug() << "jump to chat item , uid is " << si->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        auto user_info = std::make_shared<UserInfo>(si);
    }
    else
    {
        //如果没找到，则创建新的插入listwidget
        auto* chat_user_wid = new ChatUserItem();
        chat_user_wid->SetInfo(user_info);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->insertItem(0, item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
        _chat_items_added.insert(si->_uid, item);
    }

    ui->side_chat_lb->SetSelected(true);
    //跳转到聊天界面
    _current_chat_uid = user_info->_uid;
    ui->stackedWidget->setCurrentWidget(ui->chat_page);;
    //更新聊天界面信息
    SetSelectChatItem(user_info->_uid);
    ui->chat_page->SetFriendUserInfo(user_info);
    slot_side_chat();
}

void ChatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info)
{
    auto find_iter = _chat_items_added.find(user_info->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item , uid is " << user_info->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(user_info->_uid);
        //更新聊天界面信息
        ui->chat_page->SetFriendUserInfo(user_info);
        slot_side_chat();
        return;
    }

    //如果没找到，则创建新的插入listwidget
    auto* chat_user_wid = new ChatUserItem();
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    _chat_items_added.insert(user_info->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(user_info->_uid);
    //更新聊天界面信息
    ui->chat_page->SetFriendUserInfo(user_info);
    slot_side_chat();
}

void ChatDialog::slot_loading_contact_user()
{
    qDebug() << "slot loading contact user";
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    loadMoreConUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

void ChatDialog::slot_loading_chat_user()
{
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    loadMoreChatUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

void ChatDialog::slot_chat_user_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM
            || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }


   if(itemType == ListItemType::CHAT_USER_ITEM)
   {
       // 创建对话框，提示用户
       auto chat_wid = qobject_cast<ChatUserItem*>(customItem);
       chat_wid->SetSelected(true);

       // 如果点击的是同一个item，不做处理
       if (_last_selected_item == chat_wid) {
           return;
       }

       // 取消上一个选中状态
       if (_last_selected_item) {
           _last_selected_item->SetSelected(false);
       }

       // 设置新的选中状态
       chat_wid->SetSelected(true);
       _last_selected_item = chat_wid;

       auto item_user_info = chat_wid->GetUserInfo();

       qDebug()<< "当前点击的用户id：" << item_user_info->_uid;

       // 重置计数器
       int unread_count = _chat_user_unread_msg_count.value(item_user_info->_uid);
       _chat_user_unread_msg_count[item_user_info->_uid] = 0;
       _unread_msg_total_count -= unread_count;
       if (_unread_msg_total_count < 0) _unread_msg_total_count = 0;
       chat_wid->SetRedDot(false);

       if (item_user_info->_chat_msgs.empty() || chat_wid->isFirstClick())
       {
           auto userInfo = UserMgr::GetInstance()->GetUserInfo();
           item_user_info->_chat_msgs.clear();
           if (userInfo)
           {
               for (auto msg : userInfo->_chat_msgs)
               {
                   if (msg->_to_uid == item_user_info->_uid || item_user_info->_uid == msg->_from_uid)
                   {
                       item_user_info->_chat_msgs.push_back(msg);
                   }
               }
           }

           chat_wid->SetFirstClick(false);
       }

       //跳转到聊天界面
       ui->chat_page->SetFriendUserInfo(item_user_info);
       ui->chat_page->update();
       _current_chat_uid = item_user_info->_uid;
       ui->stackedWidget->setCurrentWidget(ui->chat_page);

       return;
   }
}

void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info)
{
    qDebug()<<"receive switch friend info page sig";
//    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

void ChatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata)
{
    if (_current_chat_uid == 0) {
            return;
    }

    auto find_iter = _chat_items_added.find(_current_chat_uid);
    if (find_iter == _chat_items_added.end()) {
        return;
    }

    //转为widget
    QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
    if (!widget) {
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if (itemType == CHAT_USER_ITEM)
    {
        auto con_item = qobject_cast<ChatUserItem*>(customItem);
        if (!con_item) {
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserInfo();
        user_info->_chat_msgs.push_back(msgdata);
        con_item->SetLastMsg(msgdata->_msg_content);
        con_item->SetLastMsgTime(msgdata->_send_time);

        std::vector<std::shared_ptr<TextChatData>> msg_vec;
        msg_vec.push_back(msgdata);
        UserMgr::GetInstance()->AppendFriendChatMsg(_current_chat_uid, msg_vec);
        return;
    }
}

void ChatDialog::onAuthFriendRsp(const QVariant &data)
{
    QJsonObject jsonObj = data.toJsonObject();
    if (jsonObj.isEmpty()) return;

    auto name = jsonObj["name"].toString();
    auto nick = jsonObj["nick"].toString();
    auto icon = jsonObj["icon"].toString();
    auto sex = jsonObj["sex"].toInt();
    auto uid = jsonObj["uid"].toInt();

    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int head_i = randomValue % heads.size();

    auto* chat_user_item = new ChatUserItem();

#if 1
    auto user_info = std::make_shared<UserInfo>(uid, name, nick, heads[head_i], sex);
#else
    auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);
#endif
    chat_user_item->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_item);
    _chat_items_added.insert(uid, item);
}

void ChatDialog::onNotifyAuthFriendReq(const QVariant &data)
{
    QJsonObject jsonObj = data.toJsonObject();
    if (jsonObj.isEmpty()) return;

    int from_uid = jsonObj["fromuid"].toInt();
    QString name = jsonObj["name"].toString();
    QString nick = jsonObj["nick"].toString();
    QString icon = jsonObj["icon"].toString();
    int sex = jsonObj["sex"].toInt();

    auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                nick, icon, sex);

    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int head_i = randomValue % heads.size();

#if 1
    auth_info->_icon = heads[head_i];
#endif

    auto* chat_user_wid = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_info->_uid, item);
}
