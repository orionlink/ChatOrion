#include "search_list.h"
#include "tcp_mgr.h"
#include "customize_edit.h"
#include "user_mgr.h"
#include "search_user_item.h"
#include "find_success_dialog.h"
#include "find_failed_dialog.h"
#include "common_utils.h"
#include "loading_dlg.h"
#include "message_bus.h"

#include<QScrollBar>

SearchList::SearchList(QWidget *parent):QListWidget(parent),_find_success_dlg(nullptr), _find_failed_dlg(nullptr), _send_pending(false),_search_edit_text("")
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

    MessageBus::instance()->registerHandler(MessageCommand::SEARCH_EDIT_TEXT_CHANGED, this, [this](const QVariant& data)
    {
        _search_edit_text = data.toString();
    });

    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_SEARCH_USER_RSP,
                                                   std::bind(&SearchList::searchUserRsp,
                                                   this, std::placeholders::_1,
                                                   std::placeholders::_2));
}

void SearchList::CloseFindDlg()
{
    if(_find_success_dlg)
    {
        _find_success_dlg->hide();
        _find_success_dlg = nullptr;
    }

    if (_find_failed_dlg)
    {
        _find_failed_dlg->hide();
        _find_failed_dlg = nullptr;
    }
}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool pending)
{
    if(pending){
        _loadingDialog = new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = pending;
    }else{
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
         _send_pending = pending;
    }
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
    QWidget *widget = this->itemWidget(item); //获取自定义widget对象
    if(!widget)
    {
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem)
    {
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM)
    {
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::ADD_USER_TIP_ITEM)
    {
        if (_send_pending || _search_edit_text.isEmpty())
        {
            return;
        }

        waitPending(true);

        QJsonObject jsonObj;
        jsonObj["uid"] = _search_edit_text;

        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        //发送tcp请求给chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_SEARCH_USER_REQ, jsonData);
        return;
    }

    CloseFindDlg();
}

void SearchList::searchUserRsp(int len, QByteArray data)
{
    waitPending(false);

    Q_UNUSED(len);
    // 将QByteArray转换为QJsonDocument
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    // 检查转换是否成功
    if (jsonDoc.isNull()) {
        qDebug() << "Failed to create QJsonDocument.";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("error")) {
        int err = ErrorCodes::ERR_JSON;
        qDebug() << "Login Failed, err is Json Parse Err" << err;
        return;
    }

    int err = jsonObj["error"].toInt();
    if (err != ErrorCodes::SUCCESS)
    {
        if (err == ErrorCodes::UserNotExist)
        {
            qDebug() << "查询不到该用户 err: "<< err;
            _find_failed_dlg = std::make_shared<FindFailedDialog>(this);
            _find_failed_dlg->show();
        }
        return;
    }

    auto search_info =  std::make_shared<SearchInfo>(jsonObj["uid"].toInt(), jsonObj["name"].toString(),
        jsonObj["nick"].toString(), jsonObj["desc"].toString(),
           jsonObj["sex"].toInt(), jsonObj["icon"].toString());

    //如果是自己，暂且先直接返回，以后看逻辑扩充
    auto self_uid = UserMgr::GetInstance()->GetUid();
    if (search_info->_uid == self_uid)
    {
        return;
    }
    //此处分两种情况，一种是搜多到已经是自己的朋友了，一种是未添加好友
    //查找是否已经是好友
    bool bExist = UserMgr::GetInstance()->CheckFriendById(search_info->_uid);
    if(bExist)
    {
        // 此处处理已经添加的好友，实现页面跳转
        // 跳转到聊天界面指定的item中
        emit sig_jump_chat_item(search_info);
        return;
    }

    //此处先处理为添加的好友
    _find_success_dlg = std::make_shared<FindSuccessDialog>(this);
    std::dynamic_pointer_cast<FindSuccessDialog>(_find_success_dlg)->SetSearchInfo(search_info);
    _find_success_dlg->show();
}
