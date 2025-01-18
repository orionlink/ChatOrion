#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QSet>

/**
 * @brief 聊天信息显示视图类
 */

class ChatItemBase;

class ChatView : public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget *parent = Q_NULLPTR);
    void appendChatItem(QWidget *item);
    void prependChatItem(QWidget *item);
    void insertChatItem(QWidget *before, QWidget *item);
    void removeAllItem();

    bool isInSelectionMode() const { return m_selectionMode; }
    void setSelectionMode(bool enabled);
    void clearSelection();
protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onItemSelectionChanged(ChatItemBase* item, bool selected);
    void onItemDeleteRequested(ChatItemBase* item);
    void onItemMultiSelectRequested(ChatItemBase* item);  // 处理多选请求
    void deleteSelectedItems();  // 删除选中的项目
    void onVScrollBarMoved(int min, int max);
private:
    QScrollArea *m_pScrollArea;
    bool m_isAppended;
    bool m_selectionMode;
    QSet<ChatItemBase*> m_selectedItems;
};

#endif // CHATVIEW_H
