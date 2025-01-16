#include "titlebar.h"

TitleBar::TitleBar(QWidget *parent) : QWidget(parent)
{
    initialize();
}

TitleBar::~TitleBar()
{

}

void TitleBar::setTitleText(const QString &text)
{
    m_pLabTitle->setText(text);
    this->window()->setWindowTitle(text);
}

void TitleBar::setTitleIcon(const QString &path)
{
    QPixmap pix(path);
    this->setTitleIcon(pix);
}

void TitleBar::setTitleIcon(const QPixmap &icon)
{
    if(icon.isNull()){
        return;
    }
    m_pLabIcon->setHidden(false);
    QPixmap pix = icon.scaledToHeight(m_height - 10, Qt::SmoothTransformation);
    m_pLabIcon->setPixmap(pix);
    this->window()->setWindowIcon(pix);
}

void TitleBar::setBackgroundColor(const QColor &color)
{
    m_bgColor = color;
    updateStyle();
}

void TitleBar::setTextColor(const QColor &color)
{
    m_textColor = color;
    updateStyle();
}

void TitleBar::setHeight(const uint &h)
{
    m_height = h;
    updateStyle();
}

void TitleBar::setMinIcon(const QIcon &icon)
{
    m_pBtnMin->setIcon(icon);
}

void TitleBar::setMaxIcon(const QIcon &icon)
{
    m_pBtnMax->setIcon(icon);
}

void TitleBar::setCloseIcon(const QIcon &icon)
{
    m_pBtnClose->setIcon(icon);
}

void TitleBar::setTitleTextFont(const QFont &font)
{
    m_pLabTitle->setFont(font);
}

void TitleBar::setHiddenMin(const bool &is)
{
    m_pBtnMin->setHidden(is);
}

void TitleBar::setHiddenMax(const bool &is)
{
    m_pBtnMax->setHidden(is);
}

void TitleBar::addWidget(QWidget *w)
{
    m_pCustomLayout->addWidget(w);
}

void TitleBar::addLayout(QLayout *layout)
{
    m_pCustomLayout->addLayout(layout);
}

void TitleBar::setHoverColorMin(const QColor &color)
{
    m_minHoverColor = color;
    updateStyle();
}

void TitleBar::setHoverColorMax(const QColor &color)
{
    m_maxHoverColor = color;
    updateStyle();
}

void TitleBar::setHoverColorClose(const QColor &color)
{
    m_closeHoverColor = color;
    updateStyle();
}

void TitleBar::setPressedColorMin(const QColor &color)
{
    m_minPressedColor = color;
    updateStyle();
}

void TitleBar::setPressedColorMax(const QColor &color)
{
    m_maxPressedColor = color;
    updateStyle();
}

void TitleBar::setPressedColorClose(const QColor &color)
{
    m_closePressedColor = color;
    updateStyle();
}

void TitleBar::showFull(const bool &isFull)
{
    QWidget *p = window();
    if(isFull){
        p->showFullScreen();
    }else{
        p->showMaximized();
    }
}

void TitleBar::setMoveEnable(const bool &moveEnable)
{
    m_moveEnable = moveEnable;
}

void TitleBar::setRadius(const uint &radius)
{
    m_radius = radius;
    updateStyle();
}

void TitleBar::onMax()
{
    QWidget *p = this->window();
    if(p->isMaximized()){
        p->showNormal();
        emit maxChange(false);
    }else{
        p->showMaximized();
        emit maxChange(true);
    }
}

void TitleBar::onMin()
{
    this->window()->showMinimized();
}

void TitleBar::initialize()
{
    this->setFocusPolicy(Qt::NoFocus);
    this->setFixedHeight(m_height);

    m_pBtnClose = new QPushButton(this);
    m_pBtnMax = new QPushButton(this);
    m_pBtnMin = new QPushButton(this);
    m_pLabIcon = new QLabel(this);
    m_pLabTitle = new QLabel(this);

    m_pBtnClose->setFixedSize(m_height * 2, m_height);
    m_pBtnMin->setFixedSize(m_height * 2, m_height);
    m_pBtnMax->setFixedSize(m_height * 2, m_height);

    m_pLabTitle->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_pLabTitle->setContentsMargins(5, 0, 0, 0);
    m_pLabIcon->setScaledContents(true);
    m_pLabIcon->setHidden(true);
    m_pLabIcon->setFixedHeight(m_height - 10);
    m_pBtnClose->setIcon(QIcon(":/res/close.png"));
    m_pBtnMin->setIcon(QIcon(":/res/min.png"));
    m_pBtnMax->setIcon(QIcon(":/res/max.png"));

    this->setAttribute(Qt::WA_StyledBackground);

    m_pMainLayout = new QHBoxLayout;
    m_pCustomLayout = new QHBoxLayout;
    m_pMainLayout->addWidget(m_pLabIcon);
    m_pMainLayout->addWidget(m_pLabTitle);
    m_pMainLayout->addLayout(m_pCustomLayout);
    m_pMainLayout->addStretch();
    m_pMainLayout->addWidget(m_pBtnMin);
    m_pMainLayout->addWidget(m_pBtnMax);
    m_pMainLayout->addWidget(m_pBtnClose);
    m_pMainLayout->setContentsMargins(10, 0, 0, 0);
    m_pMainLayout->setSpacing(2);
    m_pCustomLayout->setContentsMargins(0, 0, 0, 0);
    m_pCustomLayout->setSpacing(0);

    this->setLayout(m_pMainLayout);

    m_pBtnClose->setFocusPolicy(Qt::NoFocus);
    m_pBtnMin->setFocusPolicy(Qt::NoFocus);
    m_pBtnMax->setFocusPolicy(Qt::NoFocus);
    connect(m_pBtnMin,SIGNAL(clicked(bool)),this,SLOT(onMin()));
    connect(m_pBtnMax,SIGNAL(clicked(bool)),this,SLOT(onMax()));
    connect(m_pBtnClose,SIGNAL(clicked(bool)),this,SIGNAL(closed()));
}

void TitleBar::updateStyle()
{
    QString styleMin = "QPushButton{border: 0px;}QPushButton:hover{background-color: hoverColor;border-radius: 0px;}QPushButton:pressed{background-color: pressedColor;border-radius: 0px;}";
    QString styleMax = "QPushButton{border: 0px;}QPushButton:hover{background-color: hoverColor;border-radius: 0px;}QPushButton:pressed{background-color: pressedColor;border-radius: 0px;}";
    QString styleClose = "QPushButton{border: 0px;}QPushButton:hover{background-color: hoverColor;borderRadius}QPushButton:pressed{background-color: pressedColor;borderRadius}";

    this->setObjectName("titleBar");
    QString cr = QString::number(m_bgColor.red());   // 获取红色分量
    QString cg = QString::number(m_bgColor.green()); // 获取绿色分量
    QString cb = QString::number(m_bgColor.blue());  // 获取蓝色分量
    QString ca = QString::number(m_bgColor.alpha()); // 获取透明度分量
    this->setStyleSheet(QString("QWidget#titleBar{border-top-left-radius: %1px;"
                                "border-top-right-radius: %1px;"
                                "border-bottom-left-radius: 0px;"
                                "border-bottom-right-radius: 0px;"
                                "background-color: rgba(%2, %3, %4, %5);}").arg(QString::number(m_radius), cr, cg, cb, ca));
    m_pLabTitle->setStyleSheet(QString("color: %1;").arg(m_textColor.name()));

    QString borderRadius = QString("border-top-left-radius: 0px;border-top-right-radius: %1px;").arg(QString::number(m_radius));
    m_pBtnClose->setStyleSheet(styleClose.replace("hoverColor", m_closeHoverColor.name()).replace("pressedColor", m_closePressedColor.name()).replace("borderRadius", borderRadius));
    m_pBtnMax->setStyleSheet(styleMax.replace("hoverColor", m_maxHoverColor.name()).replace("pressedColor", m_maxPressedColor.name()));
    m_pBtnMin->setStyleSheet(styleMin.replace("hoverColor", m_minHoverColor.name()).replace("pressedColor", m_minPressedColor.name()));
    m_pLabIcon->setStyleSheet(QString("border-top-left-radius: %1px;").arg(QString::number(m_radius)));
    m_pLabIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    m_pLabTitle->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedHeight(m_height);
    m_pBtnClose->setFixedSize(m_height * 2, m_height);
    m_pBtnMin->setFixedSize(m_height * 2, m_height);
    m_pBtnMax->setFixedSize(m_height * 2, m_height);
    m_pLabIcon->setFixedHeight(m_height - 10);
}

void TitleBar::moveTopParent(QWidget *pWgt,QPoint movePoint)
{
    if(nullptr == pWgt){
        return;
    }

    if(pWgt->parentWidget()){
        moveTopParent(pWgt->parentWidget(),movePoint);
        return;
    }

    QPoint widgetPos = pWgt->pos();
    pWgt->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    m_isPressed = true;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_alof_win_mousePos = event->globalPos();
#else
    m_alof_win_mousePos = event->globalPosition();
#endif
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_moveEnable){
        return;
    }
    if (m_isPressed && !this->window()->isMaximized() && !this->window()->isFullScreen())
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPointF movePoint = event->globalPos() - m_alof_win_mousePos;
        m_alof_win_mousePos = event->globalPos();
#else
        QPointF movePoint = event->globalPosition() - m_alof_win_mousePos;
        m_alof_win_mousePos = event->globalPosition();
#endif
        this->window()->move(this->window()->pos() + movePoint.toPoint());
        emit movePos(movePoint);
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    QWidget::mouseReleaseEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!m_pBtnMax->isVisible() || event->button() != Qt::MouseButton::LeftButton){
        return;
    }
    onMax();
    QWidget::mouseDoubleClickEvent(event);
}
