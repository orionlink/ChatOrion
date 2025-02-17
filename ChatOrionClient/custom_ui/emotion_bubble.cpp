#include "emotion_bubble.h"

#include <QTextEdit>
#include <QMovie>

EmotionBubble::EmotionBubble(ChatRole role, const QString& emotionPath, QWidget *parent)
    : BubbleFrame(role)
{
#if 1
    // 创建 EmotionLabelItem
    m_emotionLabel = new EmotionLabelItem(this);
    // 去掉路径前缀
    QString fileNameWithExtension = emotionPath.section('/', -1);  // 获取最后一部分（emm.png）
    QString emotionName = fileNameWithExtension.section('.', 0, 0);  // 去掉后缀
    m_emotionLabel->setEmotionName(emotionName, emotionPath);  // 设置表情路径

    m_emotionLabel->startMovie();

    // 布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_emotionLabel);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);

    int left_margin = this->layout()->contentsMargins().left();
    int right_margin = this->layout()->contentsMargins().right();
    int v_margin = this->layout()->contentsMargins().bottom();
    setFixedSize(m_emotionLabel->width()+left_margin + right_margin, m_emotionLabel->height() + v_margin *2 - 3);
#endif
}
