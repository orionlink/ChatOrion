#include "chat_view.h"
#include "chat_item_base.h"
#include "message_bus.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>

ChatView::ChatView(QWidget *parent) : QWidget(parent), m_isAppended(false), m_selectionMode(false)
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
    pVLayout_1->setMargin(0);
    pVLayout_1->setSpacing(0);
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

    MessageBus::instance()->registerHandler(MessageCommand::DELETE_MULTI_SELECT_REQ, this, [this](const QVariant& data)
    {
        deleteSelectedItems();
    });

    MessageBus::instance()->registerHandler(MessageCommand::MULTI_SELECT_REQ, this, [this](const QVariant& data)
    {
        bool selected = data.toBool();
        if (!selected)
        {
            clearSelection();
        }
    });
}

void ChatView::appendChatItem(std::shared_ptr<ChatItemBase> item)
{
    if (!item) return;

    m_chatItems.insert(item->getUuid(), item);

    connect(item.get(), &ChatItemBase::deleteRequested,
            this, &ChatView::onItemDeleteRequested);
    connect(item.get(), &ChatItemBase::multiSelectRequested,
            this, &ChatView::onItemMultiSelectRequested);
    connect(item.get(), &ChatItemBase::selectionChanged,
            this, &ChatView::onItemSelectionChanged);

    QVBoxLayout *vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    if (!vl) return;

    vl->insertWidget(vl->count() - 1, item.get());
    m_isAppended = true;;
}

void ChatView::prependChatItem(std::shared_ptr<ChatItemBase> item)
{
    if (!item) return;

    m_chatItems.insert(item->getUuid(), item);

    connect(item.get(), &ChatItemBase::deleteRequested,
            this, &ChatView::onItemDeleteRequested);
    connect(item.get(), &ChatItemBase::multiSelectRequested,
            this, &ChatView::onItemMultiSelectRequested);
    connect(item.get(), &ChatItemBase::selectionChanged,
            this, &ChatView::onItemSelectionChanged);

    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    if (!vl) {
        qWarning() << "Failed to get layout from QScrollArea's widget!";
        return;
    }

    vl->insertWidget(0, item.get());
    m_isAppended = true;
}

void ChatView::insertChatItem(std::shared_ptr<ChatItemBase> before, std::shared_ptr<ChatItemBase> item)
{
    if (!item || !before) return;

    m_chatItems.insert(item->getUuid(), item);

    connect(item.get(), &ChatItemBase::deleteRequested,
            this, &ChatView::onItemDeleteRequested);
    connect(item.get(), &ChatItemBase::multiSelectRequested,
            this, &ChatView::onItemMultiSelectRequested);
    connect(item.get(), &ChatItemBase::selectionChanged,
            this, &ChatView::onItemSelectionChanged);

    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    if (!vl) {
        qWarning() << "Failed to get layout from QScrollArea's widget!";
        return;
    }

    int index = vl->indexOf(before.get());
    if (index == -1) {
        qWarning() << "The 'before' widget is not found in the layout!";
        return;
    }

    vl->insertWidget(index, item.get());
    m_isAppended = true;
}

void ChatView::removeAllItem()
{
    m_chatItems.clear();
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

void ChatView::setSelectionMode(bool enabled)
{
    m_selectionMode = enabled;
    if (!enabled) {
        clearSelection();
    }
}

void ChatView::clearSelection()
{
    if (m_chatItems.isEmpty() || m_selectedItems.isEmpty()) return;

    QSet<QString> itemsToDeselect = m_selectedItems;
    for (const auto& key : itemsToDeselect)
    {
        auto it = m_chatItems.find(key);
        if (it != m_chatItems.end() && it.value())
        {
            it.value()->setSelected(false);
        }
    }
    m_selectedItems.clear();
    m_selectionMode = false;
}

void ChatView::onItemSelectionChanged(const QString& uuid, bool selected)
{
    if (selected) {
        m_selectedItems.insert(uuid);
    } else {
        m_selectedItems.remove(uuid);
    }

    if (m_selectedItems.isEmpty())
    {
        m_selectionMode = false;
    }
    else
    {
        m_selectionMode = true;
    }
}

void ChatView::onItemDeleteRequested(const QString& uuid)
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    if (!layout) return;

    auto it = m_chatItems.find(uuid);
    if (it != m_chatItems.end())
    {
        ChatItemBase* item = it.value().get();
        disconnect(item, nullptr, this, nullptr); // 断开所有信号槽连接
        layout->removeWidget(item); // 从布局中移除
        item->hide(); // 隐藏控件
        item->setParent(nullptr); // 移除父对象
//        item->deleteLater();
//        m_chatItems.remove(uuid);
    }
}

void ChatView::onItemMultiSelectRequested(const QString& uuid)
{
    m_selectionMode = true;
    m_selectedItems.clear();  // 清除之前的选择
    m_selectedItems.insert(uuid);
}


void ChatView::deleteSelectedItems()
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    if (!layout) return;

    // 从布局和存储中移除选中的项目
    for (const auto& uuid : m_selectedItems)
    {
        std::shared_ptr<ChatItemBase> item;
        auto it = m_chatItems.find(uuid);
        if (it != m_chatItems.end())
        {
            item = it.value();
        }

        // 断开所有信号与槽连接
        disconnect(item.get(), nullptr, this, nullptr);
        layout->removeWidget(item.get());
        item->hide();
        item->setParent(nullptr);
        m_chatItems.remove(uuid);  // 从存储中移除
    }

    m_selectedItems.clear();
    m_selectionMode = false;
}
