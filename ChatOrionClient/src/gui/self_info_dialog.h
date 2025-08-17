#ifndef SELF_INFO_DIALOG_H
#define SELF_INFO_DIALOG_H

#include <QDialog>

#include "user_data.h"
#include "base_dialog.h"

namespace Ui {
class SelfInfoDialog;
}

class SelfInfoDialog : public BaseDialog
{
    Q_OBJECT

public:
    explicit SelfInfoDialog(QWidget *parent = nullptr);
    ~SelfInfoDialog();

    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
private:
    Ui::SelfInfoDialog *ui;

    std::shared_ptr<UserInfo> _user_info;
};

#endif // SELF_INFO_DIALOG_H
