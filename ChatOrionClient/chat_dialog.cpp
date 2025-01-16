#include "chat_dialog.h"
#include "ui_chat_dialog.h"
#include "chatuser_item.h"

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

        auto *chat_user_wid = new ChatUserItem();
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


    addChatUserList();
    QPixmap pixmap(":/res/pic/head_5.jpg");
    ui->side_head_lb->setPixmap(pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->side_head_lb->setScaledContents(true);

    ui->add_btn->SetState("normal","hover","press");
    ui->side_chat_lb->setProperty("state","normal");
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_setting_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

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
        //清除按钮被按下则不显示搜索框
//        ShowSearch(false);
    });

    ui->user_stacked->setCurrentIndex(2);

    load_style();
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::load_style()
{
    QString filePath = ":/res/qss/chatWin.qss";

    /*皮肤设置*/
    QFile file(filePath);/*QSS文件所在的路径*/
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet("");
    this->update();
    this->setStyleSheet(stylesheet);
    this->update();
    file.close();
}
