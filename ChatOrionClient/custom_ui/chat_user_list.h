#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>

/**
 * @brief 聊天用户信息列表
 */
class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);
signals:
    void sig_loading_chat_user();

protected:
    bool eventFilter(QObject* watched, QEvent *event) override;
private:
    bool _load_pending;
};

#endif // CHATUSERLIST_H
