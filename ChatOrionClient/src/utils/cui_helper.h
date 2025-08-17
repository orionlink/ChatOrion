#ifndef CUIHELPER_H
#define CUIHELPER_H

#include <QObject>
#include <QMenu>

#include "singleton.h"

class CUIHelper : public QObject, public Singleton<CUIHelper>
{
    Q_OBJECT
public:
    friend class Singleton<CUIHelper>;

    /*sehz
     * @brief 设置菜单圆角，配合QSS样式
     * @param menu
     */
    void setMenuRadius(QMenu* menu);
private:
    explicit CUIHelper(QObject *parent = nullptr);
private:
};

#endif // CUIHELPER_H
