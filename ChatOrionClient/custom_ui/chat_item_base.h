#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"

/**
 * 聊天信息列表项
 */
class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);
    ~ChatItemBase();

    void setUserName(const QString &name);
    void setUserIcon(const QPixmap &icon);
    void setWidget(QWidget *w);
    QWidget* getWidget() { return m_pBubble; }

    QString getUuid() const { return m_uuid; }  // 获取 UUID

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
    void deleteRequested(const QString& uuid);
    void multiSelectRequested(const QString& uuid);  // 新增多选信号
    void selectionChanged(const QString& uuid, bool selected);
private:
    void initUI();
    void setupLayout();
    void initSelectIcon();
    void registerMessageHandler();

    ChatRole m_role;
    QString m_uuid; // 表示该item唯一的标识
    QLabel *m_pNameLabel;
    QLabel *m_pIconLabel;
    QWidget *m_pBubble;
    bool m_isSelected;
    static bool m_isMultiSelected; // 是否处于多选状态

    QLabel* m_pSelectIconLabel;  // 选中状态图标
    QString m_selectedIconPath;  // 存储选中图标路径
    QString m_unselectedIconPath;// 存储未选中图标路径
};

#endif // CHATITEMBASE_H
