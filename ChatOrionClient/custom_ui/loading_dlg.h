#ifndef LOADINGDLG_H
#define LOADINGDLG_H

#include <QDialog>

class LoadingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDlg(QWidget *parent = nullptr);
    ~LoadingDlg();
};

#endif // LOADINGDLG_H
