#ifndef EMOTIONBUBBLE_H
#define EMOTIONBUBBLE_H


#include "emotion_label_item.h"
#include "bubble_frame.h"

#include <QMap>

class QTextEdit;

/**
 * @brief 表情气泡 (已弃用，请使用复合气泡 'ContentBubbleFrame' )
 */
class EmotionBubble : public BubbleFrame
{
    Q_OBJECT

public:
    EmotionBubble(ChatRole role, const QString& emotionPath, QWidget *parent = nullptr);

private:
    EmotionLabelItem* m_emotionLabel;  // 用于显示表情动画
};

#endif // EMOTIONBUBBLE_H
