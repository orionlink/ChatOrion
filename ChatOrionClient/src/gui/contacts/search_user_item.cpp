#include "search_user_item.h"
#include "ui_search_user_item.h"

SearchUserItem::SearchUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::SearchUserItem)
{
    ui->setupUi(this);

    SetItemType(ListItemType::ADD_USER_TIP_ITEM);
}

SearchUserItem::~SearchUserItem()
{
    delete ui;
}
