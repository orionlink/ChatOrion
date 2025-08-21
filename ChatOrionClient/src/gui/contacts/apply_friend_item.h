#ifndef APPLY_FRIEND_ITEM_H
#define APPLY_FRIEND_ITEM_H

#include <QWidget>
#include <memory>

#include "list_item_base.h"
#include "user_data.h"

namespace Ui {
class ApplyFriendItem;
}

/**
 * @brief 好友请求项
 */
class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem();
    void SetInfo(std::shared_ptr<ApplyInfo> apply_info);
    void ShowAddBtn(bool bshow);
    QSize sizeHint() const override {
        return QSize(250, 80); // 返回自定义的尺寸
    }
    int GetUid();
private:
    Ui::ApplyFriendItem *ui;
    std::shared_ptr<ApplyInfo> _apply_info;
    bool _added;
signals:
    void sig_auth_friend(std::shared_ptr<ApplyInfo> apply_info);
};

#endif // APPLY_FRIEND_ITEM_H
