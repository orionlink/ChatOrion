#ifndef CHATUSER_ITEM_H
#define CHATUSER_ITEM_H

#include "list_item_base.h"

namespace Ui {
class ChatUserItem;
}

class RedDotLabel;

/**
 * @brief 聊天用户列表项
 */
class ChatUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();

    QSize sizeHint() const override {
        return QSize(250, 70); // 返回自定义的尺寸
    }

    void SetInfo(QString name, QString head, QString msg);

    void SetRedDot(bool show = false, int count = 0);
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateRedDotPosition();

    Ui::ChatUserItem *ui;

    QString _name;
    QString _head;
    QString _msg;

    RedDotLabel* redDotLabel;
};

#endif // CHATUSER_ITEM_H
