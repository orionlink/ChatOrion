#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <QDialog>
#include <memory>

#include "user_data.h"

class LoadingDlg;

/**
 * @brief 搜索列表
 */
class SearchList: public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget *parent = nullptr);
    void CloseFindDlg();
    std::shared_ptr<QDialog> getFindDialog() { return _find_dlg; }
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void waitPending(bool pending = true);
    void addTipItem();

    /**
     * @brief 搜索用户回包处理
     * @param len
     * @param data
     */
    void searchUserRsp(int len, QByteArray data);
private:
    bool _send_pending;
    std::shared_ptr<QDialog> _find_dlg;
    QString _search_edit_text; // 搜索框内容
    LoadingDlg * _loadingDialog;
private slots:
    void slot_item_clicked(QListWidgetItem *item);
signals:
    void sig_jump_chat_item(std::shared_ptr<SearchInfo> si);
};

#endif // SEARCHLIST_H
