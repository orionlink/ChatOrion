#include "cui_helper.h"

#include <QEvent>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

CUIHelper::CUIHelper(QObject *parent) : QObject(parent)
{

}

void CUIHelper::setMenuRadius(QMenu* menu)
{
    menu->setWindowFlag(Qt::FramelessWindowHint);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlag(Qt::NoDropShadowWindowHint);
}
