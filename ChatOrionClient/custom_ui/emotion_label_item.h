#ifndef EMOTIONLABELITEM_H
#define EMOTIONLABELITEM_H

#include "clicked_label.h"
#include "bubble_frame.h"

class QMovie;

/**
 * @brief 表情包项(可点击)
 */
class EmotionLabelItem : public ClickedLabel
{
	Q_OBJECT

public:
    EmotionLabelItem(QWidget *parent = nullptr);
	~EmotionLabelItem();

    void setEmotionName(const QString& emotionName, const QString& emotionPath);
    void startMovie();
signals:
    void emotionClicked(const QString& emotionPath);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
	void initControl();

private:
    QString m_emotionName;
    QString m_emotionPath;
	QMovie* m_apngMovie;
};

#endif // EMOTIONLABELITEM_H
