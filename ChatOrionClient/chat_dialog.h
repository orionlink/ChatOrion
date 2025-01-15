#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
private:
    void load_style();
private:
    Ui::ChatDialog *ui;
};

#endif // CHAT_DIALOG_H
