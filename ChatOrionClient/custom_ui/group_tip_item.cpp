#include "group_tip_item.h"
#include "ui_group_tip_item.h"

GroupTipItem::GroupTipItem(QWidget *parent) :ListItemBase (parent),_tip(""),
    ui(new Ui::GroupTipItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::GROUP_TIP_ITEM);
}

GroupTipItem::~GroupTipItem()
{
    delete ui;
}


QSize GroupTipItem::sizeHint() const
{
    return QSize(250, 25); // 返回自定义的尺寸
}

void GroupTipItem::SetGroupTip(QString str)
{
    ui->label->setText(str);
}
