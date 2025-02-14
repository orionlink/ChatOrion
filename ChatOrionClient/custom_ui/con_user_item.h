#ifndef CON_USER_ITEM_H
#define CON_USER_ITEM_H

#include <QWidget>

#include "list_item_base.h"
#include "user_data.h"

namespace Ui {
class ConUserItem;
}

class RedDotLabel;

/**
 * @brief 联系人项
 */
class ConUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();
    QSize sizeHint() const override;
    void SetInfo(std::shared_ptr<AuthInfo> auth_info);
    void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void SetInfo(int uid, QString name, QString icon);
//    void ShowRedPoint(bool show = false);

    void SetRedDot(bool show, int count = 0);
    std::shared_ptr<UserInfo> GetInfo();
private:
    void resizeEvent(QResizeEvent *event) override;
    void updateRedDotPosition();

    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _info;
    RedDotLabel* _redDotLabel;
};

#endif // CON_USER_ITEM_H
