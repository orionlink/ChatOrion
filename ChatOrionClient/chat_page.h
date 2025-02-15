#ifndef CHAT_PAGE_H
#define CHAT_PAGE_H

#include <QWidget>

#include "clicked_label.h"
#include "emotion_window.h"
#include "user_data.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

    void SetFriendUserInfo(std::shared_ptr<UserInfo> user_info);

    void AppendChatMsg(std::shared_ptr<TextChatData> msg);
signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);

protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_send_btn_clicked();

    /**
     * @brief 按下表情图标, 显示表情窗口
     * @param text
     * @param state
     */
    void onEmoLabelClicked(QString text, ClickLbState state);

    /**
     * @brief 按下某个表情
     * @param emoji_path 表情资源文件路径
     */
    void onEmotionItemClicked(const QString& emoji_path);

    void onDeleteLabelClicked();

    void onCloseLabelClicked();
private:

    /**
     * @brief 根据内容创建一个 ChatItemBase
     * @param user_info
     * @param group
     * @param isPicture 是否是图片消息
     * @return 返回一个 ChatItemBase 的uuid，可作为消息id
     */
    QString createAndAppendChatItem(std::shared_ptr<UserInfo> user_info, const QVector<MsgInfo>& group, bool isPicture = false);
private:
    Ui::ChatPage *ui;
    EmotionWindow* _emotion_wid;
    std::shared_ptr<UserInfo> _friend_user_info; // 记录当前对话的用户信息
};

#endif // CHAT_PAGE_H
