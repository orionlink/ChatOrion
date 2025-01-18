#include "chat_page.h"
#include "ui_chat_page.h"
#include "chat_item_base.h"
#include "text_bubble.h"
#include "picture_bubble.h"
#include "emotion_bubble.h"
#include "content_bubble.h"

#include <QStyleOption>
#include <QPainter>

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

    ui->chat_msg_btn->setToolTip(QStringLiteral("聊天信息"));
    ui->emo_lb->setToolTip(QStringLiteral("表情"));
    ui->file_lb->setToolTip(QStringLiteral("发送文件"));

    QObject::connect(ui->emo_lb, &ClickedLabel::clicked, this, &ChatPage::onEmoLabelClicked);
    QObject::connect(_emotion_wid, &EmotionWindow::signalEmotionItemClicked, this, &ChatPage::onEmotionItemClicked);
}

ChatPage::~ChatPage()
{
    delete ui;
    _emotion_wid->deleteLater();
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
//    auto pTextEdit = ui->chatEdit;
//    ChatRole role = ChatRole::Self;
//    QString userName = QStringLiteral("test");
//    QString userIcon = ":/res/pic/head_1.jpg";

//    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
//    QWidget *pBubble = nullptr;
//    ChatItemBase *pChatItem = new ChatItemBase(role);
//    pChatItem->setUserName(userName);
//    pChatItem->setUserIcon(QPixmap(userIcon));

//    ContentBubbleFrame* bubble = new ContentBubbleFrame(ChatRole::Self);

//    bool success = bubble->setContent(msgList);
//    if (success)
//    {
//         pChatItem->setWidget(bubble);
//         ui->chat_data_list->appendChatItem(pChatItem);
//    }


//    for (auto iter = msgList.begin(); iter != msgList.end(); iter++)
//    {
//        if (iter->msgFlag != "image") continue;

//        ChatItemBase *pChatItem1 = new ChatItemBase(role);
//        pChatItem1->setUserName(userName);
//        pChatItem1->setUserIcon(QPixmap(userIcon));

//        pBubble = new PictureBubble(QPixmap(iter->content), ChatRole::Self);

//        if(pBubble)
//        {
//            ui->chat_data_list->appendChatItem(pChatItem1);
//            pChatItem1->setWidget(pBubble);
//        }
//    }

    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("test");
    QString userIcon = ":/res/pic/head_1.jpg";

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    if (msgList.isEmpty()) return;

    // 使用新的消息分组处理函数
    handleGroupedMessages(msgList, role, userName, userIcon);

    // 清空输入框
    pTextEdit->clear();
}

// 消息分组处理函数
void ChatPage::handleGroupedMessages(const QVector<MsgInfo>& msgList, ChatRole role,
                                     const QString& userName, const QString& userIcon)
{
    if (msgList.isEmpty()) return;

    QVector<MsgInfo> currentGroup;
    bool hasImage = false;

    auto createAndAppendChatItem = [this, role, userName, userIcon](const QVector<MsgInfo>& group, bool isPicture = false) {
        if (group.isEmpty()) return;

        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));

        QWidget *pBubble = nullptr;
        if (isPicture) {
            // 创建图片气泡
            pBubble = new PictureBubble(QPixmap(group.first().content), role);
        } else {
            // 创建内容气泡
            ContentBubbleFrame* bubble = new ContentBubbleFrame(role);
            bool success = bubble->setContent(group);
            if (!success) return;
            pBubble = bubble;
        }

        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    };

    // 遍历消息列表
    for (int i = 0; i < msgList.size(); ++i) {
        const MsgInfo& msg = msgList[i];

        if (msg.msgFlag == "image") {
            // 如果当前组有内容，先创建内容气泡
            if (!currentGroup.isEmpty()) {
                createAndAppendChatItem(currentGroup);
                currentGroup.clear();
            }

            // 创建单独的图片气泡
            QVector<MsgInfo> imageGroup;
            imageGroup.append(msg);
            createAndAppendChatItem(imageGroup, true);
        } else {
            // 文本或表情消息，添加到当前组
            currentGroup.append(msg);
        }
    }

    // 处理最后一组消息
    if (!currentGroup.isEmpty()) {
        createAndAppendChatItem(currentGroup);
    }
}

void ChatPage::onEmoLabelClicked(QString text, ClickLbState state)
{
    if (state == ClickLbState::Selected)
    {
        _emotion_wid->setVisible(!_emotion_wid->isVisible());
        QPoint emotionPoint = this->mapToGlobal(ui->emo_lb->pos());
        emotionPoint.setX(emotionPoint.x());
        emotionPoint.setY(emotionPoint.y());
        _emotion_wid->move(emotionPoint);
    }
}

void ChatPage::onEmotionItemClicked(const QString &emoji_path)
{
    ui->chatEdit->setFocus();
    ui->chatEdit->addEmotionUrl(emoji_path);
}
