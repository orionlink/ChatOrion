#include "contact_user_list.h"
#include "global.h"
#include "list_item_base.h"
#include "group_tip_item.h"
#include "con_user_item.h"
#include "tcp_mgr.h"
#include "user_mgr.h"

#include <QRandomGenerator>
#include <QTimer>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

/************************测试使用　开始**************************/
inline std::vector<QString>  test_strs ={"你好",
                             "在干嘛",
                             "好累呀",
                            "You have to love yourself",
                            "今天有大新闻"};

inline std::vector<QString> heads = {
    ":/res/pic/head_1.jpg",
    ":/res/pic/head_2.jpg",
    ":/res/pic/head_3.jpg",
    ":/res/pic/head_4.jpg",
    ":/res/pic/head_5.jpg"
};

inline std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};

ContactUserList::ContactUserList(QWidget *parent): _add_friend_item(nullptr)
  ,_load_pending(false)
{
    Q_UNUSED(parent);
     this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
     this->viewport()->installEventFilter(this);

    //模拟从数据库或者后端传输过来的数据,进行列表加载
    addContactUserList();
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);
//    //链接对端同意认证后通知的信号
    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_NOTIFY_AUTH_FRIEND_REQ,
                                                   std::bind(&ContactUserList::NotifyAuthFriendReq,
                                                   this, std::placeholders::_1, std::placeholders::_2));

    TcpMgr::GetInstance()->registerMessageCallback(ReqId::ID_AUTH_FRIEND_RSP,
                                                   std::bind(&ContactUserList::AuthFriendRsp,
                                                   this, std::placeholders::_1, std::placeholders::_2));
}


void ContactUserList::SetRedDot(bool show, int count)
{
    _add_friend_item->SetRedDot(show, count);
}

void ContactUserList::addContactUserList()
{
    //获取好友列表
    auto * groupTip = new GroupTipItem();
    groupTip->SetGroupTip(QStringLiteral("新的朋友"));
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    _add_friend_item = new ConUserItem();
    _add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->SetInfo(0,tr("新的朋友"),":/res/pic/add_friend.png");
    _add_friend_item->SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    QListWidgetItem *add_item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    add_item->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item, _add_friend_item);
    //默认设置新的朋友申请条目被选中
    this->setCurrentItem(add_item);

    auto * groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("联系人"));
    _groupitem = new QListWidgetItem;
    _groupitem->setSizeHint(groupCon->sizeHint());
    this->addItem(_groupitem);
    this->setItemWidget(_groupitem, groupCon);
    _groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);

//    //加载后端发送过来的好友列表
    auto con_list = UserMgr::GetInstance()->GetConListPerPage();
    for(auto & con_ele : con_list){
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(con_ele->_uid,con_ele->_name, con_ele->_icon);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }

    UserMgr::GetInstance()->UpdateContactLoadedCount();

    // 模拟列表， 创建QListWidgetItem，并设置自定义的widget
#if 0
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%test_strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();

        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(0,names[name_i], heads[head_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
#endif
}

void ContactUserList::NotifyAuthFriendReq(int len, QByteArray data)
{
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
        qDebug() << "Auth Friend Failed, err is " << err;
        return;
    }

    int err = jsonObj["error"].toInt();
    if (err != ErrorCodes::SUCCESS) {
        qDebug() << "Auth Friend Failed, err is " << err;
        return;
    }

    int from_uid = jsonObj["fromuid"].toInt();
    QString name = jsonObj["name"].toString();
    QString nick = jsonObj["nick"].toString();
    QString icon = jsonObj["icon"].toString();
    int sex = jsonObj["sex"].toInt();

    auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                nick, icon, sex);

    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if(isFriend){
        return;
    }

    // 在 groupitem 之后插入新项
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int str_i = randomValue% test_strs.size();

#if 1
    auth_info->_icon = heads[randomValue% heads.size()];
#endif

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_info);
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取 groupitem 的索引
    int index = this->row(_groupitem);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);

    this->setItemWidget(item, con_user_wid);
}

void ContactUserList::AuthFriendRsp(int len, QByteArray data)
{
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
        qDebug() << "Auth Friend Failed, err is Json Parse Err" << err;
        return;
    }

    int err = jsonObj["error"].toInt();
    if (err != ErrorCodes::SUCCESS) {
        qDebug() << "Auth Friend Failed, err is " << err;
        return;
    }

    auto name = jsonObj["name"].toString();
    auto nick = jsonObj["nick"].toString();
    auto icon = jsonObj["icon"].toString();
    auto sex = jsonObj["sex"].toInt();
    auto uid = jsonObj["uid"].toInt();

    auto auth_rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);

    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if(isFriend){
        return;
    }
    // 在 groupitem 之后插入新项
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int str_i = randomValue%test_strs.size();
    int head_i = randomValue%heads.size();

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_rsp->_uid ,auth_rsp->_name, heads[head_i]);
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取 groupitem 的索引
    int index = this->row(_groupitem);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);

    this->setItemWidget(item, con_user_wid);
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量

        if (maxScrollValue - currentValue <= 0) {

//            auto b_loaded = UserMgr::GetInstance()->IsLoadChatFin();
//            if(b_loaded){
//                return true;
//            }

//            if(_load_pending){
//                return true;
//            }

//            _load_pending = true;

//            QTimer::singleShot(100, [this](){
//                _load_pending = false;
//                QCoreApplication::quit(); // 完成后退出应用程序
//                });
            // 滚动到底部，加载新的联系人
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_contact_user();
         }

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);

}

void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM
            || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

   if(itemType == ListItemType::APPLY_FRIEND_ITEM){

       // 创建对话框，提示用户
       qDebug()<< "apply friend item clicked ";
       //跳转到好友申请界面
       emit sig_switch_apply_friend_page();
       return;
   }

   if(itemType == ListItemType::CONTACT_USER_ITEM){
       // 创建对话框，提示用户
       qDebug()<< "contact user item clicked ";

       auto con_item = qobject_cast<ConUserItem*>(customItem);
       auto user_info = con_item->GetInfo();

       emit sig_switch_friend_info_page(user_info);
       return;
   }
}


