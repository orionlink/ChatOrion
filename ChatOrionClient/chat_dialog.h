#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

#include "FrameWgt/framewgt.h"

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
private:
    void load_style();

    // 测试函数
    void addChatUserList();
private:
    Ui::ChatDialog *ui;
};

#endif // CHAT_DIALOG_H
