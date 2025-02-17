#ifndef AUTHEN_FRIEND_DIALOG_H
#define AUTHEN_FRIEND_DIALOG_H

#include <QDialog>
#include <QMap>

#include "user_data.h"
#include "clicked_label.h"

namespace Ui {
class AuthenFriendDialog;
}

class FriendLabel;

class AuthenFriendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenFriendDialog(QWidget *parent = nullptr);
    ~AuthenFriendDialog();
    void InitTipLbs();
    void AddTipLbs(ClickedLabel*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    void SetApplyInfo(std::shared_ptr<ApplyInfo> apply_info);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    void resetLabels();
    void addLabel(QString name);

    //已经创建好的标签
    QMap<QString, ClickedLabel*> _add_labels;
    std::vector<QString> _add_label_keys;
    QPoint _label_point;
    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabel*> _friend_labels;
    std::vector<QString> _friend_label_keys;
    std::vector<QString> _tip_data;
    QPoint _tip_cur_point;

    std::shared_ptr<SearchInfo> _search_info; // 搜索到的信息

    std::shared_ptr<ApplyInfo> _apply_info;
    Ui::AuthenFriendDialog *ui;
public slots:
    //显示更多label标签
    void ShowMoreLabel();
    //输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString);
    //通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString, ClickLbState);
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
   //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();
};

#endif // AUTHEN_FRIEND_DIALOG_H
