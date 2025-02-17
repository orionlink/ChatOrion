#include "emotion_label_item.h"

#include <QMovie>
#include <QResizeEvent>

EmotionLabelItem::EmotionLabelItem(QWidget *parent )
    : ClickedLabel(parent), m_emotionName(""), m_emotionPath("")
{
	initControl();
    connect(this, &ClickedLabel::clicked, [this](){
        emit emotionClicked(m_emotionPath);
	});
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);
	setObjectName("emotionLabelItem");
    setFixedSize(32, 32);
}

EmotionLabelItem::~EmotionLabelItem()
{

}

void EmotionLabelItem::startMovie()
{
    if (m_apngMovie)
    {
        m_apngMovie->start();  // 开始播放动画
    }
}

void EmotionLabelItem::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);

    // 当 QLabel 大小变化时，调整动画的大小
    if (m_apngMovie) {
        m_apngMovie->setScaledSize(event->size());
    }
}

void EmotionLabelItem::setEmotionName(const QString& emotionName, const QString& emotionPath)
{
    m_emotionName = emotionName;
    m_emotionPath = emotionPath;

    m_apngMovie = new QMovie(m_emotionPath, "apng", this);

    // 设置动画大小适应 QLabel 的大小
    m_apngMovie->setScaledSize(size());

    // 将动画设置到 QLabel
    setMovie(m_apngMovie);

    // 开始播放动画
    m_apngMovie->start();
    m_apngMovie->stop();  // 停止动画，等待鼠标悬停时播放

    setToolTip(emotionName);
}

void EmotionLabelItem::enterEvent(QEvent *event)
{
    if (m_apngMovie)
    {
        m_apngMovie->start();  // 开始播放动画
    }
    QWidget::enterEvent(event);
}

void EmotionLabelItem::leaveEvent(QEvent *event)
{
    if (m_apngMovie)
    {
        m_apngMovie->jumpToFrame(0);  // 跳转到第一帧
        m_apngMovie->stop();          // 停止动画
    }
    QWidget::leaveEvent(event);
}
