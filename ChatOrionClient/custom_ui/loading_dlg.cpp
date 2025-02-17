#include "loading_dlg.h"
#include <QLabel>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QScreen>
#include <QHBoxLayout>

LoadingDlg::LoadingDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    // 获取屏幕尺寸
    setFixedSize(parent->size()); // 设置对话框为全屏尺寸

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    this->setLayout(mainLayout);

    QLabel * loading_lb = new QLabel(this);
    mainLayout->addWidget(loading_lb);

    QMovie *movie = new QMovie(":/res/pic/loading.gif"); // 加载动画的资源文件
    loading_lb->setMovie(movie);
    movie->start();
}

LoadingDlg::~LoadingDlg()
{
}
