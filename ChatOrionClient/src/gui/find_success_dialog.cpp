#include "find_success_dialog.h"
#include "ui_find_success_dialog.h"
#include "apply_friend_dialog.h"

#include <QDir>
#include <QMouseEvent>

FindSuccessDialog::FindSuccessDialog(QWidget *parent) :
    BaseDialog(parent),
    ui(new Ui::FindSuccessDialog)
    ,_parent(parent)
{
    ui->setupUi(this);

    // 设置对话框标题
    setWindowTitle("添加");
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path +
                            QDir::separator() + "static"+QDir::separator()+"head_1.jpg");
    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(),
           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    ui->add_friend_btn->SetState("normal","hover","press");
}

FindSuccessDialog::~FindSuccessDialog()
{
    delete ui;
}

void FindSuccessDialog::SetSearchInfo(std::shared_ptr<SearchInfo> search_info)
{
    ui->name_lb->setText(search_info->_name);
    _search_info = search_info;
}

void FindSuccessDialog::on_add_friend_btn_clicked()
{
    this->hide();
    //弹出加好友界面
    auto applyFriend = new ApplyFriend(_parent);
    applyFriend->SetSearchInfo(_search_info);
    applyFriend->show();
}
