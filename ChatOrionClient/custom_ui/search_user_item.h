#ifndef SEARCH_USER_ITEM_H
#define SEARCH_USER_ITEM_H

#include "list_item_base.h"

namespace Ui {
class SearchUserItem;
}

class SearchUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit SearchUserItem(QWidget *parent = nullptr);
    ~SearchUserItem();

private:
    Ui::SearchUserItem *ui;
};

#endif // SEARCH_USER_ITEM_H
