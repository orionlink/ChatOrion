#include "chat_dialog.h"
#include "ui_chat_dialog.h"
#include "chat_user_item.h"
#include "common_utils.h"

#include <QFile>
#include <QTextStream>
#include <QAction>
#include <QRandomGenerator>

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

void ChatDialog::addChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 50; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % test_strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();

        int msg_count = QRandomGenerator::global()->bounded(110);

        bool is_show = QRandomGenerator::global()->bounded(2);
        auto *chat_user_wid = new ChatUserItem();
        chat_user_wid->SetRedDot(is_show, msg_count);
        chat_user_wid->SetInfo(names[name_i], heads[head_i], test_strs[str_i]);
        QListWidgetItem *item = new QListWidgetItem();
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}

/************************测试使用 结束**************************/

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("微信"));

    ui->add_btn->SetState("normal","hover","press");
    ui->side_chat_lb->setProperty("state","normal");
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_setting_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    ui->side_chat_lb->setToolTip(QStringLiteral("聊天"));
    ui->side_contact_lb->setToolTip(QStringLiteral("联系人"));
    ui->side_setting_lb->setToolTip(QStringLiteral("设置及其他"));

    // 显示搜索图标
    QAction* searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/pic/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    ui->search_edit->setMaxLength(15);

    // 添加清除按钮
    QAction* clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/pic/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    QObject::connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text)
    {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/res/pic/close_search.png"));
        } else {
            clearAction->setIcon(QIcon(":/res/pic/close_transparent.png")); // 文本为空时，切换回透明图标
        }
    });

    QObject::connect(clearAction, &QAction::triggered, [this, clearAction]
    {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/res/pic/close_transparent.png")); // 清除文本后，切换回透明图标
        ui->search_edit->clearFocus();
    });

    addLBGroup(ui->side_chat_lb);
    addLBGroup(ui->side_contact_lb);

    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_search_edit_text_changed);

    //连接联系人页面点击好友申请条目的信号
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this,&ChatDialog::slot_switch_apply_friend_page);

    ui->user_stacked->setCurrentWidget(ui->chat_user_list_page);
    ui->stackedWidget->setCurrentWidget(ui->normal_page);

    CommonUtils::loadStyleSheet(this, "chatWin");

    installEventFilter(this);

    /// 测试使用
    addChatUserList();
    QPixmap pixmap(":/res/pic/head_5.jpg");
    ui->side_head_lb->setPixmap(pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->side_head_lb->setScaledContents(true);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::addLBGroup(StateWidget *lb)
{
    _side_lb_list.push_back(lb);
}

void ChatDialog::clearLabelState(StateWidget *lb)
{
    for(auto & ele: _side_lb_list){
        if(ele == lb){
            continue;
        }

        ele->ClearState();
    }
}

bool ChatDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        if (mouse_event)
        {
            if (ui->user_stacked->currentWidget() == ui->search_list_page)
            {
                // 将鼠标点击位置转换为搜索列表坐标系中的位置
                QPoint posInSearchList = ui->search_list->mapFromGlobal(mouse_event->globalPos());
                // 判断点击位置是否在聊天列表的范围内
                if (!ui->search_list->rect().contains(posInSearchList))
                {
                    ui->search_edit->clear(); // 只需清空即可，会触发slot_search_edit_text_changed函数实现切换逻辑
                }
            }

            QPoint posInDialog = mouse_event->globalPos();

            // 判断点击位置是否在 QDialog 的范围内
            auto find_dlg = ui->search_list->getFindDialog();
            if (find_dlg)
            {
                if (!find_dlg->geometry().contains(posInDialog)) {
                    ui->search_list->CloseFindDlg();
                    qDebug() << "Dialog closed because click was outside.";
                }
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}

void ChatDialog::slot_side_chat()
{
    clearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    ui->user_stacked->setCurrentWidget(ui->chat_user_list_page);
}

void ChatDialog::slot_side_contact()
{
    clearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->normal_page);
    ui->user_stacked->setCurrentWidget(ui->con_user_list_page);
}

void ChatDialog::slot_search_edit_text_changed()
{
    // 搜索页面索引为0， 消息页面为1，联系人为2
    static int user_stacked_last_index = 0;
    if (!ui->search_edit->text().isEmpty())
    {
        if (user_stacked_last_index == 0) user_stacked_last_index = ui->user_stacked->currentIndex();

        ui->user_stacked->setCurrentWidget(ui->search_list_page);
    }
    else
    {
        ui->user_stacked->setCurrentIndex(user_stacked_last_index);
        user_stacked_last_index = 0;
    }
}

void ChatDialog::slot_switch_apply_friend_page()
{
    ui->stackedWidget->setCurrentIndex(1);
}
