#ifndef FIND_FAILED_DIALOG_H
#define FIND_FAILED_DIALOG_H

#include <QDialog>

#include "base_dialog.h"

namespace Ui {
class FindFailedDialog;
}

class FindFailedDialog : public BaseDialog
{
    Q_OBJECT

public:
    explicit FindFailedDialog(QWidget *parent = nullptr);
    ~FindFailedDialog();
private slots:
    void on_fail_sure_btn_clicked();
private:
    Ui::FindFailedDialog *ui;
};

#endif // FIND_FAILED_DIALOG_H
