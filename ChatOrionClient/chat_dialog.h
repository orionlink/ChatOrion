#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

#include "FrameWgt/framewgt.h"

class StateWidget;

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
private:
    /**
     * @brief 添加到按钮组，支持互斥点击
     * @param lb
     */
    void addLBGroup(StateWidget *lb);

    /**
     * @brief 清理按钮状态，即按钮点击状态变为正常状态
     * @param lb 除了该按钮，其他全部恢复
     */
    void clearLabelState(StateWidget *lb);

    bool eventFilter(QObject *obj, QEvent *event) override;

    // 测试函数
    void addChatUserList();
private slots:
    /**
     * @brief 点击侧边聊天
     */
    void slot_side_chat();

    /**
     * @brief 点击侧边联系人
     */
    void slot_side_contact();

    /**
     * @brief 搜索框文本修改槽函数
     */
    void slot_search_edit_text_changed();

    /**
     * @brief 跳转到好友申请界面
     */
    void slot_switch_apply_friend_page();
private:
    Ui::ChatDialog *ui;
    QList<StateWidget*> _side_lb_list;
};

#endif // CHAT_DIALOG_H
