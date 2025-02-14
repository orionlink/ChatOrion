#include "find_failed_dialog.h"
#include "ui_find_failed_dialog.h"

#include <QDebug>
#include <QDir>
#include <QMouseEvent>

FindFailedDialog::FindFailedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindFailedDialog)
{
    ui->setupUi(this);

    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path +
                            QDir::separator() + "static"+QDir::separator()+"head_1.jpg");

    ui->fail_sure_btn->SetState("normal","hover","press");
}

FindFailedDialog::~FindFailedDialog()
{
    qDebug() << "FindFailedDialog 已经被销毁";
    delete ui;
}

void FindFailedDialog::on_fail_sure_btn_clicked()
{
    this->close();
}
