#ifndef CHATUSER_ITEM_H
#define CHATUSER_ITEM_H

#include "list_item_base.h"
#include "user_data.h"

namespace Ui {
class ChatUserItem;
}

class RedDotLabel;

/**
 * @brief 聊天用户列表项
 */
class ChatUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();

    QSize sizeHint() const override {
        return QSize(250, 70); // 返回自定义的尺寸
    }

    void SetInfo(QString name, QString head, QString msg, QString last_msg = "");

    void SetInfo(std::shared_ptr<UserInfo> user_info);
    void SetInfo(std::shared_ptr<FriendInfo> friend_info);

    void SetRedDot(bool show = false, int count = 0);

    std::shared_ptr<UserInfo> GetUserInfo();

    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);

    void SetLastMsg(const QString &msg);

    void SetSelected(bool selected) { _selected = selected; }
    bool isSelected() { return _selected; }
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateRedDotPosition();

    Ui::ChatUserItem *ui;

    RedDotLabel* redDotLabel;

    std::shared_ptr<UserInfo> _user_info;

    bool _selected = false;
};

#endif // CHATUSER_ITEM_H
