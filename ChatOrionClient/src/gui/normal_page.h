#ifndef NORMAL_PAGE_H
#define NORMAL_PAGE_H

#include <QWidget>

namespace Ui {
class NormalPage;
}

class NormalPage : public QWidget
{
    Q_OBJECT

public:
    explicit NormalPage(QWidget *parent = nullptr);
    ~NormalPage();

private:
    Ui::NormalPage *ui;
};

#endif // NORMAL_PAGE_H
