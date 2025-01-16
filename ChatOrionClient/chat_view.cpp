#include "chat_view.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

#include <QTimer>
#include <QStyleOption>
#include <QPainter>

ChatView::ChatView(QWidget *parent) : QWidget(parent), m_isAppended(false)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);
    pMainLayout->setMargin(0);

    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    pMainLayout->addWidget(m_pScrollArea);

    QWidget *w = new QWidget(this);
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);
    QVBoxLayout *pVLayout_1 = new QVBoxLayout();
    // 默认往里面加一个widget(item)，每条消息(item)就会被顶到顶部，如果没有拉伸因子
    // 会导致一条消息占满整个界面, 消息是一个widget
    pVLayout_1->addWidget(new QWidget(), 100000);
    w->setLayout(pVLayout_1);
    m_pScrollArea->setWidget(w);    //应该时在QSCrollArea构造后执行 才对

    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    connect(pVScrollBar, &QScrollBar::rangeChanged,this, &ChatView::onVScrollBarMoved);
    //把垂直ScrollBar放到上边 而不是原来的并排
    QHBoxLayout *pHLayout_2 = new QHBoxLayout();
    pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
    pHLayout_2->setMargin(0);
    m_pScrollArea->setLayout(pHLayout_2);
    pVScrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
}

void ChatView::appendChatItem(QWidget *item)
{
   QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
   qDebug() << "vl->count() is " << vl->count();
   vl->insertWidget(vl->count() - 1, item);
   m_isAppended = true;
}

void ChatView::prependChatItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    if (!vl)
    {
        qWarning() << "Failed to get layout from QScrollArea's widget!";
        return;
    }
    vl->insertWidget(0, item);
    m_isAppended = true;
}

void ChatView::insertChatItem(QWidget *before, QWidget *item)
{
    // 获取 QScrollArea 内容控件的布局管理器
    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    if (!vl)
    {
        qWarning() << "Failed to get layout from QScrollArea's widget!";
        return;
    }

    // 查找 before 控件在布局中的索引
    int index = vl->indexOf(before);
    if (index == -1)
    {
        qWarning() << "The 'before' widget is not found in the layout!";
        return;
    }

    // 在 before 控件之前插入新的聊天项
    vl->insertWidget(index, item);

    // 标记已添加聊天项
    m_isAppended = true;
}

void ChatView::removeAllItem()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());

   int count = layout->count();

    for (int i = 0; i < count - 1; ++i)
    {
        QLayoutItem *item = layout->takeAt(0); // 始终从第一个控件开始删除
        if (item)
        {
            QWidget *widget = item->widget();
            if (widget)
            {
                delete widget;
            }
            delete item;
        }
    }

}

bool ChatView::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Enter && obj == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }
    else if(event->type() == QEvent::Leave && obj == m_pScrollArea)
    {
         m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(obj, event);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if(m_isAppended) //添加item可能调用多次
    {
        QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        //500毫秒内可能调用多次
        QTimer::singleShot(500, [this]()
        {
            m_isAppended = false;
        });
    }
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
