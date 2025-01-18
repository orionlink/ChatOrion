#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);
    void setUserName(const QString &name);
    void setUserIcon(const QPixmap &icon);
    void setWidget(QWidget *w);
    QWidget* getWidget() { return m_pBubble; }

    // 选择状态管理
    bool isSelected() const { return m_isSelected; }
    void setSelected(bool selected);
    static void setIsMultiSelected(bool selected) { m_isMultiSelected = selected; }
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void showContextMenu(const QPoint& pos);
signals:
    void deleteRequested(ChatItemBase* item);
    void multiSelectRequested(ChatItemBase* item);  // 新增多选信号
    void selectionChanged(ChatItemBase* item, bool selected);
private:
    ChatRole m_role;
    QLabel *m_pNameLabel;
    QLabel *m_pIconLabel;
    QWidget *m_pBubble;
    bool m_isSelected;
    static bool m_isMultiSelected; // 是否处于多选状态

    QLabel* m_pSelectIconLabel;  // 选中状态图标
    void initSelectIcon();       // 初始化图标
    QString m_selectedIconPath;  // 存储选中图标路径
    QString m_unselectedIconPath;// 存储未选中图标路径
};

#endif // CHATITEMBASE_H
