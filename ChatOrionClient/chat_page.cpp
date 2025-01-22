#include "chat_page.h"
#include "ui_chat_page.h"
#include "chat_item_base.h"
#include "text_bubble.h"
#include "picture_bubble.h"
#include "emotion_bubble.h"
#include "content_bubble.h"
#include "message_bus.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>

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

    ui->stackedWidget->setCurrentWidget(ui->input_page);
    MessageBus::instance()->registerHandler(MessageCommand::MULTI_SELECT_REQ, this, [this](const QVariant& data)
    {
        bool selected = data.toBool();
        if (selected)
            ui->stackedWidget->setCurrentWidget(ui->action_page);
        else
            ui->stackedWidget->setCurrentWidget(ui->input_page);
    });

    QObject::connect(ui->emo_lb, &ClickedLabel::clicked, this, &ChatPage::onEmoLabelClicked);
    QObject::connect(ui->delete_lb, &ClickedLabel::clicked, this, &ChatPage::onDeleteLabelClicked);
    QObject::connect(ui->action_close_lb, &ClickedLabel::clicked, this, &ChatPage::onCloseLabelClicked);
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

        auto pChatItem = std::make_unique<ChatItemBase>(role);
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
        ui->chat_data_list->appendChatItem(std::move(pChatItem));
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

void ChatPage::onDeleteLabelClicked()
{
    MessageBus::sendMessage(MessageCommand::DELETE_MULTI_SELECT_REQ, false);
}

void ChatPage::onCloseLabelClicked()
{
    MessageBus::sendMessage(MessageCommand::MULTI_SELECT_REQ, false);
}
