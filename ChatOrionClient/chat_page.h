#ifndef CHAT_PAGE_H
#define CHAT_PAGE_H

#include <QWidget>

#include "clicked_label.h"
#include "emotion_window.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_send_btn_clicked();

    /**
     * @brief 按下表情图标, 显示表情窗口
     * @param text
     * @param state
     */
    void onEmoLabelClicked(QString text, ClickLbState state);

    /**
     * @brief 按下某个表情
     * @param emoji_path 表情资源文件路径
     */
    void onEmotionItemClicked(const QString& emoji_path);
private:
    void handleGroupedMessages(const QVector<MsgInfo>& msgList, ChatRole role,
                                         const QString& userName, const QString& userIcon);
private:
    Ui::ChatPage *ui;
    EmotionWindow* _emotion_wid;
};

#endif // CHAT_PAGE_H
