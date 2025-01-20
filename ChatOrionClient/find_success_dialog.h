#ifndef FIND_SUCCESS_DIALOG_H
#define FIND_SUCCESS_DIALOG_H

#include <QDialog>

#include "user_data.h"

namespace Ui {
class FindSuccessDialog;
}

class FindSuccessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDialog(QWidget *parent = nullptr);
    ~FindSuccessDialog();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);
private slots:
    void on_add_friend_btn_clicked();
private:
    Ui::FindSuccessDialog *ui;
    QWidget * _parent;
    std::shared_ptr<SearchInfo> _si;
};

#endif // FIND_SUCCESS_DIALOG_H
