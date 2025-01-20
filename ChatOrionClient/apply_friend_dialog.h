#ifndef APPLY_FRIEND_DIALOG_H
#define APPLY_FRIEND_DIALOG_H

#include <QDialog>
#include <QMap>

#include "clicked_label.h"
#include "friend_label.h"
#include "user_data.h"

namespace Ui {
class ApplyFriendDialog;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();
private:
    Ui::ApplyFriendDialog *ui;
};


#endif // APPLY_FRIEND_DIALOG_H
