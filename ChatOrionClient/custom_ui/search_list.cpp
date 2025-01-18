#include "search_list.h"
#include "tcp_mgr.h"
#include "customize_edit.h"
#include "user_mgr.h"
#include "search_user_item.h"

#include<QScrollBar>

SearchList::SearchList(QWidget *parent):QListWidget(parent),_find_dlg(nullptr), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
     this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
//    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
//    if(_find_dlg){
//        _find_dlg->hide();
//        _find_dlg = nullptr;
//    }
}

void SearchList::SetSearchEdit(QWidget* edit) {
    _search_edit = edit;
}

void SearchList::waitPending(bool pending)
{
}


void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);

    auto *search_user_item = new SearchUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(search_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, search_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
}
