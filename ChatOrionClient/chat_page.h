#ifndef CHAT_PAGE_H
#define CHAT_PAGE_H

#include <QWidget>

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_send_btn_clicked();

private:
    Ui::ChatPage *ui;
};

#endif // CHAT_PAGE_H
