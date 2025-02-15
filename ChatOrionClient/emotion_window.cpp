#include "emotion_window.h"
#include "common_utils.h"
#include "emotion_label_item.h"
#include "user_data.h"

#include <QPainter>
#include <QFocusEvent>
#include <QMovie>
#include <QScrollBar>
#include <QStyleOption>
#include <cmath>
#include <QDir>

EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);

	ui.setupUi(this);
	initControl();
}

EmotionWindow::~EmotionWindow()
{

}

void EmotionWindow::initControl()
{
	ui.scrollArea->verticalScrollBar()->setSingleStep(32);

	CommonUtils::loadStyleSheet(this, "EmotionWindow");

    bool is_load = EmojiManager::GetInstance()->loadEmojiData(global_emoji_name);
    if (!is_load) return;

    QJsonObject emoObject = EmojiManager::GetInstance()->getEmojiObject();

    if (emoObject.isEmpty()) return;

    int row = 0, column = 0;
    int total_count = 0;
    for (auto iter = emoObject.begin(); iter != emoObject.end(); iter++)
    {
        QString emo_name = iter.key();
        QString emo_path = iter.value().toString();

        EmotionLabelItem* emo_label = new EmotionLabelItem(this);
        emo_label->setEmotionName(emo_name, emo_path);
        connect(emo_label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
        ui.gridLayout->addWidget(emo_label, row, column++);
        if (column == 10)
        {
            row++;
            column = 0;
        }

        total_count++;
    }
}

void EmotionWindow::addEmotion(const QString& emotionPath)
{
	hide();
	emit signalEmotionWindowHide();
    emit signalEmotionItemClicked(emotionPath);
}

void EmotionWindow::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void EmotionWindow::showEvent(QShowEvent *event)
{
	setFocus();
    QWidget::showEvent(event);
}

void EmotionWindow::focusOutEvent(QFocusEvent *event)
{
	QWidget* widget = qApp->widgetAt(QCursor::pos());
	if (widget != this && widget != ui.emotionWidget && widget != ui.topWidget)
	{
		hide();
		emit signalEmotionWindowHide();
	}

    QWidget::focusOutEvent(event);
}
