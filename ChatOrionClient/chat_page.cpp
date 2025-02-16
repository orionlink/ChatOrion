#include "chat_page.h"
#include "ui_chat_page.h"
#include "chat_item_base.h"
#include "text_bubble.h"
#include "picture_bubble.h"
#include "emotion_bubble.h"
#include "content_bubble.h"
#include "message_bus.h"
#include "user_mgr.h"
#include "tcp_mgr.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QScreen>
#include <QJsonDocument>
#include <QDateTime>

ChatPage::ChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage),
    _emotion_wid(new EmotionWindow())
{
    ui->setupUi(this);
    _emotion_wid->hide();

    //设置按钮样式
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");

    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
    ui->telphone_lb->SetState("normal","hover","press","normal","hover","press");
    ui->video_lb->SetState("normal","hover","press","normal","hover","press");

    ui->chat_msg_btn->setToolTip(QStringLiteral("聊天信息"));
    ui->emo_lb->setToolTip(QStringLiteral("表情"));
    ui->file_lb->setToolTip(QStringLiteral("发送文件"));
    ui->telphone_lb->setToolTip(QStringLiteral("语音聊天"));
    ui->video_lb->setToolTip(QStringLiteral("视频聊天"));

    ui->item_forward_lb->SetState("normal","hover","press","normal","hover","press");
    ui->item_forward_lb->setToolTip(QStringLiteral("逐条转发"));

    ui->merge_forward_lb->SetState("normal","hover","press","normal","hover","press");
    ui->merge_forward_lb->setToolTip(QStringLiteral("合并转发"));

    ui->save_lb->SetState("normal","hover","press","normal","hover","press");
    ui->save_lb->setToolTip(QStringLiteral("保存"));

    ui->delete_lb->SetState("normal","hover","press","normal","hover","press");
    ui->delete_lb->setToolTip(QStringLiteral("删除"));

    ui->collect_lb->SetState("normal","hover","press","normal","hover","press");
    ui->collect_lb->setToolTip(QStringLiteral("收藏"));

    ui->action_close_lb->SetState("normal","hover","press","normal","hover","press");

    ui->receive_btn->hide();

    ui->stackedWidget->setCurrentWidget(ui->input_page);
    MessageBus::instance()->registerHandler(MessageCommand::MULTI_SELECT_REQ, this, [this](const QVariant& data)
    {
        bool selected = data.toBool();
        if (selected)
            ui->stackedWidget->setCurrentWidget(ui->action_page);
        else
            ui->stackedWidget->setCurrentWidget(ui->input_page);
    });


    QObject::connect(ui->chatEdit, &MessageTextEdit::send, this, &ChatPage::on_send_btn_clicked);

    QObject::connect(ui->emo_lb, &ClickedLabel::clicked, this, &ChatPage::onEmoLabelClicked);
    QObject::connect(ui->delete_lb, &ClickedLabel::clicked, this, &ChatPage::onDeleteLabelClicked);
    QObject::connect(ui->action_close_lb, &ClickedLabel::clicked, this, &ChatPage::onCloseLabelClicked);
    QObject::connect(_emotion_wid, &EmotionWindow::signalEmotionItemClicked, this, &ChatPage::onEmotionItemClicked);
    QObject::connect(ui->chatEdit, &MessageTextEdit::textChanged, this, [this](){
        QString text = ui->chatEdit->toPlainText();
        if (text.isEmpty())
        {
            ui->send_btn->setObjectName("send_btn");
        }
        else
        {
            ui->send_btn->setObjectName("send_activa_btn");
        }

        repolish(ui->send_btn);
    });

    connect(this, &ChatPage::sig_append_send_chat_msg, this, &ChatPage::slot_append_send_chat_msg);
}

ChatPage::~ChatPage()
{
    delete ui;
    if (_emotion_wid)
        _emotion_wid->deleteLater();
}

void ChatPage::SetFriendUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _friend_user_info = user_info;
    ui->username_label->setText(user_info->_name);

    int userId = user_info->_uid;
    ChatViewData& viewData = _chat_view_cache[userId];

    if (!viewData.view)
    {
        viewData.view = new ChatView(ui->chat_data_wid);
        viewData.view->setObjectName("chat_data_list");
        if (ui->chat_data_list) {
            viewData.view->setSizePolicy(ui->chat_data_list->sizePolicy());
        }
    }

    // 更新布局
    QVBoxLayout* dataLayout = qobject_cast<QVBoxLayout*>(ui->chat_data_wid->layout());
    if (dataLayout) {
        if (ui->chat_data_list) {
            ui->chat_data_list->hide();
            dataLayout->removeWidget(ui->chat_data_list);
        }

        dataLayout->addWidget(viewData.view);
        ui->chat_data_list = viewData.view;
    }

    // 检查消息是否需要更新
    bool needUpdate = viewData.messages.size() != user_info->_chat_msgs.size();
//    if (!needUpdate) {
//        // 如果大小相同，逐个比较确认是否需要更新
//        for (size_t i = 0; i < viewData.messages.size(); ++i) {
//            if (viewData.messages[i] != user_info->_chat_msgs[i]) {
//                needUpdate = true;
//                break;
//            }
//        }
//    }

    if (needUpdate)
    {
        viewData.messages.clear();
        viewData.messages = user_info->_chat_msgs;
        viewData.view->removeAllItem();
        for(auto & msg : viewData.messages) {
            AppendChatMsg(msg);
        }
    }

//    if (forcedUpdate)
//    {
//        viewData.view->removeAllItem();
//        for(auto & msg : user_info->_chat_msgs) {
//            AppendChatMsg(msg);
//        }
//    }

    viewData.view->show();
    viewData.view->update();
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role;
    //todo... 添加聊天显示
    if (msg->_from_uid == self_info->_uid) {
        role = ChatRole::Self;
        auto pChatItem = std::make_unique<ChatItemBase>(role);

        pChatItem->setUserName(self_info->_name);

#if 1
        self_info->_icon = ":/res/pic/head_3.jpg";
#endif

        pChatItem->setUserIcon(QPixmap(self_info->_icon));
        ContentBubbleFrame* pBubble = nullptr;
        pBubble = new ContentBubbleFrame(role);
        pBubble->setContent(msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(std::move(pChatItem));
    }
    else {
        role = ChatRole::Other;
        auto pChatItem = std::make_unique<ChatItemBase>(role);
        auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
        if (friend_info == nullptr)
        {
            return;
        }
        pChatItem->setUserName(friend_info->_name);
        pChatItem->setUserIcon(QPixmap(friend_info->_icon));
        ContentBubbleFrame* pBubble = nullptr;
        pBubble = new ContentBubbleFrame(role);
        pBubble->setContent(msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(std::move(pChatItem));
    }
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_clicked()
{
    auto pTextEdit = ui->chatEdit;

    auto user_info = UserMgr::GetInstance()->GetUserInfo();

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    if (msgList.isEmpty()) return;

    QVector<MsgInfo> currentGroup;

    QJsonObject textObj;
    QString content = "";

    // 遍历消息列表
    for (int i = 0; i < msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024) continue;

        const MsgInfo& msg = msgList[i];

        if (msg.msgFlag == "image")
        {
            // 如果当前组有内容，先创建内容气泡
            if (!currentGroup.isEmpty())
            {
                QString msgid = createAndAppendChatItem(user_info, currentGroup, false);

                textObj["from_uid"] = user_info->_uid;
                textObj["to_uid"] = _friend_user_info->_uid;
                textObj["msg_id"] = msgid;
                textObj["content"] = content;

                QDateTime currentDateTime = QDateTime::currentDateTime();
                qint64 send_time = currentDateTime.currentSecsSinceEpoch();
                textObj["send_time"] = send_time;
                textObj["msg_type"] = MessageType::TEXT;

                qDebug() << "消息发送的文本内容: " << content;

                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

                emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);

                auto txt_msg = std::make_shared<TextChatData>(msgid, content,
                                user_info->_uid, _friend_user_info->_uid, send_time, MessageType::TEXT, 0);
                emit sig_append_send_chat_msg(txt_msg);

                //发送并清空之前累计的文本
                textObj = QJsonObject();
                content = "";

                currentGroup.clear();
            }

            // 创建单独的图片气泡
            QVector<MsgInfo> imageGroup;
            imageGroup.append(msg);
            createAndAppendChatItem(user_info, imageGroup, true);
        }
        else if (msg.msgFlag == "text")
        {
            QByteArray utf8Message = msg.content.toUtf8();
            content += QString::fromUtf8(utf8Message);
            // 文本或表情消息，添加到当前组
            currentGroup.append(msg);
        }
        else if(msg.msgFlag == "emotion")
        {
            QByteArray utf8Message = msg.content.toUtf8();
            QString emotion_path = QString::fromUtf8(utf8Message);
            int startIndex = emotion_path.lastIndexOf('/') + 1;  // 找到最后一个 '/' 的位置
            int endIndex = emotion_path.lastIndexOf('.');        // 找到最后一个 '.' 的位置
            QString result = emotion_path.mid(startIndex, endIndex - startIndex);  // 提取中间部分
            result.prepend("[");
            result.append("]");

            content += result;

            currentGroup.append(msg);
        }
    }

    qDebug() << "消息发送的文本内容: " << content;

    // 处理最后一组消息
    if (!currentGroup.isEmpty())
    {
        QString msgid = createAndAppendChatItem(user_info, currentGroup, false);

        textObj["from_uid"] = user_info->_uid;
        textObj["to_uid"] = _friend_user_info->_uid;
        textObj["msg_id"] = msgid;
        textObj["content"] = content;

        QDateTime currentDateTime = QDateTime::currentDateTime();
        qint64 send_time = currentDateTime.currentSecsSinceEpoch();
        textObj["send_time"] = send_time;
        textObj["msg_type"] = MessageType::TEXT;

        QJsonDocument doc(textObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);

        auto txt_msg = std::make_shared<TextChatData>(msgid, content,
                        user_info->_uid, _friend_user_info->_uid, send_time, MessageType::TEXT, 0);
        emit sig_append_send_chat_msg(txt_msg);

        //发送并清空之前累计的文本列表
        textObj = QJsonObject();
    }

    // 清空输入框
    pTextEdit->clear();
}

QString ChatPage::createAndAppendChatItem(std::shared_ptr<UserInfo> user_info,const QVector<MsgInfo> &group, bool isPicture)
{
    QString uuid = "";
    if (group.isEmpty() || !user_info) return uuid;

    ChatRole role = ChatRole::Self;
    auto pChatItem = std::make_unique<ChatItemBase>(role);
    pChatItem->setUserName(user_info->_name);
    pChatItem->setUserIcon(QPixmap(user_info->_icon));

    QWidget *pBubble = nullptr;
    if (isPicture) {
        // 创建图片气泡
        pBubble = new PictureBubble(QPixmap(group.first().content), role);
    } else {
        // 创建内容气泡
        ContentBubbleFrame* bubble = new ContentBubbleFrame(role);
        bool success = bubble->setContent(group);
        if (!success) return uuid;
        pBubble = bubble;
    }

    pChatItem->setWidget(pBubble);
    uuid = pChatItem->getUuid();

    ui->chat_data_list->appendChatItem(std::move(pChatItem));

    return uuid;
}

void ChatPage::onEmoLabelClicked(QString text, ClickLbState state)
{
    if (state == ClickLbState::Selected)
    {
        _emotion_wid->setVisible(!_emotion_wid->isVisible());

        // 获取按钮的全局位置
        QPoint btnGlobalPos = ui->emo_lb->mapToGlobal(QPoint(0,0));

        // 获取表情窗口的大小
        QSize emoSize = _emotion_wid->size();

        // 计算表情窗口位置
        // x: 按钮的左边对齐
        // y: 按钮的顶部位置 减去 表情窗口的高度 再往上偏移一点(比如5像素)
        QPoint emotionPoint(
            btnGlobalPos.x(),  // 左对齐
            btnGlobalPos.y() - emoSize.height() - 5  // 在按钮上方
        );

        _emotion_wid->move(emotionPoint);
    }
}

void ChatPage::onEmotionItemClicked(const QString &emoji_path)
{
    ui->chatEdit->setFocus();
    ui->chatEdit->addEmotionUrl(emoji_path);
}

void ChatPage::onDeleteLabelClicked()
{
    MessageBus::sendMessage(MessageCommand::DELETE_MULTI_SELECT_REQ, false);
}

void ChatPage::onCloseLabelClicked()
{
    MessageBus::sendMessage(MessageCommand::MULTI_SELECT_REQ, false);
}

void ChatPage::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata)
{
    if (msgdata == nullptr) return;

    _chat_view_cache[msgdata->_to_uid].messages.push_back(msgdata);
}

void ChatPage::slot_notify_text_chat_msg(std::vector<std::shared_ptr<TextChatData> > chat_msgs)
{
    if (chat_msgs.empty()) return;

    for (auto msg : chat_msgs)
    {
        _chat_view_cache[msg->_from_uid].messages.push_back(msg);
    }
}
