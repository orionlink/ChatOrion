#include "chat_dialog.h"
#include "ui_chat_dialog.h"

#include <QFile>
#include <QTextStream>

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);

    ui->add_btn->SetState("normal","hover","press");
    ui->side_chat_lb->setProperty("state","normal");

    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

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
