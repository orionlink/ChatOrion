#include "frameless_wid_drag_mgr.h"

void FramelessWidDragMgr::registerWindow(QWidget *window)
{
    if (!window) return;

    // 设置无边框窗口
    window->setWindowFlags(window->windowFlags() | Qt::FramelessWindowHint);

    // 安装事件过滤器
    window->installEventFilter(this);

    // 存储窗口的拖动状态
    dragStates[window] = false;
}

bool FramelessWidDragMgr::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *window = qobject_cast<QWidget *>(obj);
    if (!window || !dragStates.contains(window)) {
        return QObject::eventFilter(obj, event);
    }

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (mouseEvent->button() == Qt::LeftButton) {
            dragStates[window] = true;
            dragStartPositions[window] = mouseEvent->globalPos() - window->frameGeometry().topLeft();
            window->setCursor(Qt::ClosedHandCursor); // 设置拖动光标
        }
        break;

    case QEvent::MouseMove:
        if (dragStates[window] && (mouseEvent->buttons() & Qt::LeftButton)) {
            window->move(mouseEvent->globalPos() - dragStartPositions[window]);
        }
        break;

    case QEvent::MouseButtonRelease:
        if (mouseEvent->button() == Qt::LeftButton) {
            dragStates[window] = false;
            window->unsetCursor(); // 恢复默认光标
        }
        break;

    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}
