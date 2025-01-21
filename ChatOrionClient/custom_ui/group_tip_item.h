#ifndef GROUP_TIP_ITEM_H
#define GROUP_TIP_ITEM_H

#include <QWidget>

#include "list_item_base.h"

namespace Ui {
class GroupTipItem;
}

/**
 * @brief 联系人分组项，用于划分
 */
class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();
    QSize sizeHint() const override;
    void SetGroupTip(QString str);
private:
    QString _tip;
    Ui::GroupTipItem *ui;
};

#endif // GROUP_TIP_ITEM_H
