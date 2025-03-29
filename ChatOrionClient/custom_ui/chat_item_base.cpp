#include "chat_item_base.h"
#include "chat_view.h"
#include "bubble_frame.h"
#include "message_bus.h"
#include "cui_helper.h"

#include <QFont>
#include <QVBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QUuid>

bool ChatItemBase::m_isMultiSelected = false;

ChatItemBase::ChatItemBase(ChatRole role, QWidget *parent)
    : QWidget(parent)
    , m_role(role)
    , m_uuid(QUuid::createUuid().toString())
    , m_isSelected(false)
{

    initUI();
    setupLayout();
    initSelectIcon();
    registerMessageHandler();

    qDebug() << "ChatItemBase::ChatItemBase";
}

ChatItemBase::~ChatItemBase()
{
    qDebug() << "ChatItemBase::~ChatItemBase()";
}

void ChatItemBase::initSelectIcon()
{
    // 默认显示未选中状态图标
    m_selectedIconPath = ":/res/pic/selected.png";     // 绿色选中图标路径
    m_unselectedIconPath = ":/res/pic/unselected.png"; // 白色未选中图标路径

    m_pSelectIconLabel->setPixmap(QPixmap(m_unselectedIconPath));
}

void ChatItemBase::registerMessageHandler()
{
    MessageBus::instance()->registerHandler(MessageCommand::MULTI_SELECT_REQ, this, [this](const QVariant& data) {
        bool selected = data.toBool();
        m_isMultiSelected = selected;
        m_pSelectIconLabel->setVisible(m_isMultiSelected);
    });
}

void ChatItemBase::setUserName(const QString &name)
{
    m_pNameLabel->setText(name);
}

void ChatItemBase::setUserIcon(const QPixmap &icon)
{
    m_pIconLabel->setPixmap(icon);
}

void ChatItemBase::setWidget(QWidget *w)
{
   QGridLayout *pGLayout = (qobject_cast<QGridLayout *>)(this->layout());
   pGLayout->replaceWidget(m_pBubble, w);
   delete m_pBubble;
   m_pBubble = w;

   // 如果新的部件是 BubbleFrame，建立连接
   if (BubbleFrame* bubble = qobject_cast<BubbleFrame*>(w)) {
       connect(bubble, &BubbleFrame::bubbleContextMenuRequested,
               this, &ChatItemBase::showContextMenu);
   }
}

void ChatItemBase::setSelected(bool selected)
{
    if (m_isSelected != selected)
    {
        m_isSelected = selected;
    }
    else
    {
        m_isSelected = false;
    }

    // 更新图标
    m_pSelectIconLabel->setPixmap(QPixmap(m_isSelected ? m_selectedIconPath : m_unselectedIconPath));
    m_pSelectIconLabel->setVisible(m_isMultiSelected);

    update();
    emit selectionChanged(m_uuid, m_isSelected);
}

void ChatItemBase::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}

void ChatItemBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_isSelected) {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(180, 180, 180, 50));
    }
}

void ChatItemBase::mousePressEvent(QMouseEvent *event)
{
    if (m_isMultiSelected && event->button() == Qt::LeftButton) {
        setSelected(true);  // 在多选模式下，点击直接选中
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void ChatItemBase::showContextMenu(const QPoint& pos)
{
    QMenu menu(this);

    CUIHelper::GetInstance()->setMenuRadius(&menu);


    QAction* copyAction = menu.addAction(tr("复制"));
    QAction* zoomAction = menu.addAction(tr("放大阅读"));
    QAction* forwardAction = menu.addAction(tr("转发"));
    QAction* collectAction = menu.addAction(tr("收藏"));
    QAction* multiSelectAction = menu.addAction(tr("多选"));
    QAction* deleteAction = menu.addAction(tr("删除"));
    QAction* result = menu.exec(pos);

    if (result == multiSelectAction)
    {
        m_isMultiSelected = true;
        m_isSelected = true;

        // 显示所有项的图标
        QList<ChatItemBase*> items = parentWidget()->findChildren<ChatItemBase*>();
        for (ChatItemBase* item : items) {
            item->m_pSelectIconLabel->setVisible(true);
            item->m_pSelectIconLabel->setPixmap(QPixmap(item->m_unselectedIconPath));
        }

        // 更新当前项的图标为选中状态
        m_pSelectIconLabel->setPixmap(QPixmap(m_selectedIconPath));

        MessageBus::sendMessage(MessageCommand::MULTI_SELECT_REQ, true);

        emit multiSelectRequested(m_uuid);
    }
    else if (result == deleteAction)
    {
//        MessageBus::sendMessage(MessageCommand::DELETE_SINGLE_SELECT_REQ, m_uuid);
        emit deleteRequested(m_uuid);
    }
}

void ChatItemBase::initUI()
{
    m_pNameLabel = new QLabel();
    m_pNameLabel->setObjectName("chat_user_name");
    QFont font("Microsoft YaHei");
    font.setPointSize(9);
    m_pNameLabel->setFont(font);
    m_pNameLabel->setFixedHeight(20);

    m_pIconLabel = new QLabel();
    m_pIconLabel->setScaledContents(true);
    m_pIconLabel->setFixedSize(42, 42);

    m_pBubble = new QWidget();

    m_pSelectIconLabel = new QLabel(this);
    m_pSelectIconLabel->setFixedSize(16, 16);
    m_pSelectIconLabel->setScaledContents(true);
    m_pSelectIconLabel->setVisible(false);  // 默认隐藏
}

void ChatItemBase::setupLayout()
{
    QGridLayout *pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);
    pGLayout->setHorizontalSpacing(3);
    pGLayout->setMargin(3);
    QSpacerItem* pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    if (m_role == ChatRole::Self) {
        m_pNameLabel->setContentsMargins(0, 0, 8, 0);
        m_pNameLabel->setAlignment(Qt::AlignRight);
        pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);
        pGLayout->addWidget(m_pIconLabel, 0, 2, 2, 1, Qt::AlignTop);
        pGLayout->addItem(pSpacer, 1, 0, 1, 1);
        pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);
        pGLayout->setColumnStretch(0, 2);
        pGLayout->setColumnStretch(1, 3);
        pGLayout->addWidget(m_pSelectIconLabel, 0, 0, 2, 1, Qt::AlignVCenter);
    } else {
        m_pNameLabel->setContentsMargins(8, 0, 0, 0);
        m_pNameLabel->setAlignment(Qt::AlignLeft);
        pGLayout->addWidget(m_pIconLabel, 0, 1, 2, 1, Qt::AlignVCenter);
        pGLayout->addWidget(m_pNameLabel, 0, 2, 1, 1);
        pGLayout->addWidget(m_pBubble, 1, 2, 1, 1);
        pGLayout->addItem(pSpacer, 1, 3, 1, 1);
        pGLayout->setColumnStretch(1, 3);
        pGLayout->setColumnStretch(2, 2);
        pGLayout->addWidget(m_pSelectIconLabel, 0, 0, 2, 1, Qt::AlignVCenter);
    }

    // 单聊不显示用户名
    m_pNameLabel->hide();
    this->setLayout(pGLayout);
}
