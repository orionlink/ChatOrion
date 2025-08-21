#ifndef FRAMELESSWINDOWDRAGMANAGER_H
#define FRAMELESSWINDOWDRAGMANAGER_H

#include <QObject>
#include <QHash>
#include <QPoint>
#include <QWidget>
#include <QMouseEvent>

#include "singleton.h"

/**
 * @brief 无边框窗口拖动管理
 */
class FramelessWidDragMgr : public QObject, public Singleton<FramelessWidDragMgr>
{
    Q_OBJECT

public:
    friend class Singleton<FramelessWidDragMgr>;

    // 注册窗口
    void registerWindow(QWidget *window);

protected:
    // 事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // 私有构造函数
    FramelessWidDragMgr(QObject *parent = nullptr) : QObject(parent) {}

    // 存储窗口的拖动状态
    QHash<QWidget *, bool> dragStates;

    // 存储窗口的拖动起始位置
    QHash<QWidget *, QPoint> dragStartPositions;
};

#endif // FRAMELESSWINDOWDRAGMANAGER_H
