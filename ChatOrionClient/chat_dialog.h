#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

#include "FrameWgt/framewgt.h"
#include "user_data.h"

class StateWidget;
class QListWidgetItem;
class ChatUserItem;

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

signals:
    void sig_notify_text_chat_msg(std::vector<std::shared_ptr<TextChatData>> chat_msgs);
private:
    /**
     * @brief 添加到按钮组，支持互斥点击
     * @param lb
     */
    void addLBGroup(StateWidget *lb);

    /**
     * @brief 清理按钮状态，即按钮点击状态变为正常状态
     * @param lb 除了该按钮，其他全部恢复
     */
    void clearLabelState(StateWidget *lb);

    void NotifyAddFriendReq(int len, QByteArray data);

    /**
     * @brief 接收好友发送的消息
     * @param len
     * @param data
     */
    void NotifyTextChatMsgReq(int len, QByteArray data);

    bool eventFilter(QObject *obj, QEvent *event) override;

    void addChatUserList();

    void loadMoreChatUser();

    void loadMoreConUser();

    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata);

    void SetSelectChatItem(int uid = 0);
private slots:
    /**
     * @brief 点击侧边聊天
     */
    void slot_side_chat();

    /**
     * @brief 点击侧边联系人
     */
    void slot_side_contact();

    void slot_side_collect();

    void slot_setting_label();

    /**
     * @brief 搜索框文本修改槽函数
     */
    void slot_search_edit_text_changed();

    /**
     * @brief 跳转到好友申请界面
     */
    void slot_switch_apply_friend_page();

    /**
     * @brief 连接searchlist跳转聊天信号
     * @param si
     */
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);

    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);

    /**
     * @brief 加载联系人
     */
    void slot_loading_contact_user();

    /**
     * @brief 加载聊天用户
     */
    void slot_loading_chat_user();

    void slot_chat_user_item_clicked(QListWidgetItem *item);

    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);

    /**
     * @brief 发送消息，将消息和用户信息保存在 ChatUserItem 中
     * @param msgdata
     */
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);

    void onAuthFriendRsp(const QVariant& data);
    void onNotifyAuthFriendReq(const QVariant& data);
private:
    Ui::ChatDialog *ui;
    QList<StateWidget*> _side_lb_list;
    // 保存聊天用户列表项
    QMap<int, QListWidgetItem*> _chat_items_added; // key: uid, value: ChatUserItem
    int _current_chat_uid;
    bool _b_loading;

    int _unread_msg_total_count; // 当前消息的总数
    // key: uid, value: unread_msg_count
    QMap<int, int> _chat_user_unread_msg_count; // 关联和当前好友未读消息的数量

    ChatUserItem* _last_selected_item = nullptr; // 记录上一次点击的item
};

#endif // CHAT_DIALOG_H
