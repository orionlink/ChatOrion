#include "normal_page.h"
#include "ui_normal_page.h"

NormalPage::NormalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalPage)
{
    ui->setupUi(this);
}

NormalPage::~NormalPage()
{
    delete ui;
}
