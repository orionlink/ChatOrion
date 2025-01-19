#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QMutex>

/**
 * @brief 聊天信息显示视图类
 */

class ChatItemBase;

// 自定义 qHash 函数
inline size_t qHash(const std::shared_ptr<ChatItemBase>& ptr, size_t seed = 0) noexcept {
    return qHash(ptr.get(), seed); // 使用原始指针的地址计算哈希值
}

class ChatView : public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget *parent = Q_NULLPTR);
    void appendChatItem(std::shared_ptr<ChatItemBase> item);
    void prependChatItem(std::shared_ptr<ChatItemBase> item);
    void insertChatItem(std::shared_ptr<ChatItemBase> before, std::shared_ptr<ChatItemBase> item);
    void removeAllItem();

    bool isInSelectionMode() const { return m_selectionMode; }
    void setSelectionMode(bool enabled);
    void clearSelection();
protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onItemSelectionChanged(const QString& uuid, bool selected);
    void onItemDeleteRequested(const QString& uuid);
    void onItemMultiSelectRequested(const QString& uuid);  // 处理多选请求
    void deleteSelectedItems();  // 删除选中的项目
    void onVScrollBarMoved(int min, int max);
private:
    QScrollArea *m_pScrollArea;
    bool m_isAppended;
    bool m_selectionMode;

    QMap<QString, std::shared_ptr<ChatItemBase>> m_chatItems;  // 存储所有聊天项
    QSet<QString> m_selectedItems;  // 存储选中的 UUID
};

#endif // CHATVIEW_H
