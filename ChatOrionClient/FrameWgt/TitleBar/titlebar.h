#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QStyle>

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

    void setTitleText(const QString &text);
    void setTitleIcon(const QString &path);
    void setTitleIcon(const QPixmap &icon);
    void setBackgroundColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setHeight(const uint &h);
    void setMinIcon(const QIcon &icon);
    void setMaxIcon(const QIcon &icon);
    void setCloseIcon(const QIcon &icon);
    void setTitleTextFont(const QFont &font);
    void setHiddenMin(const bool &is);
    void setHiddenMax(const bool &is);
    void addWidget(QWidget *w);
    void addLayout(QLayout *layout);
    void setHoverColorMin(const QColor &color);
    void setHoverColorMax(const QColor &color);
    void setHoverColorClose(const QColor &color);
    void setPressedColorMin(const QColor &color);
    void setPressedColorMax(const QColor &color);
    void setPressedColorClose(const QColor &color);

    void showFull(const bool &isFull);
    void setMoveEnable(const bool &moveEnable);
    void setRadius(const uint &radius);
public slots:
    void onMax();
    void onMin();
signals:
    void closed();
    void movePos(QPointF movePoint);
    void maxChange(bool is);
protected:
    void initialize();
    void updateStyle();

    void moveTopParent(QWidget *pWgt, QPoint movePoint);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QHBoxLayout *m_pMainLayout = nullptr;
    QHBoxLayout *m_pCustomLayout = nullptr;
    QPushButton *m_pBtnClose = nullptr;
    QPushButton *m_pBtnMax = nullptr;
    QPushButton *m_pBtnMin = nullptr;
    QLabel *m_pLabIcon = nullptr;
    QLabel *m_pLabTitle = nullptr;

    bool m_isPressed = false;
    bool m_moveEnable = true;
    QPointF m_alof_win_mousePos = QPoint(0,0);
    uint m_height = 30;
    QColor m_bgColor = Qt::white;
    QColor m_textColor = Qt::black;
    uint m_radius = 10;
    QColor m_minHoverColor = QColor(240, 240, 240);
    QColor m_maxHoverColor = QColor(240, 240, 240);
    QColor m_closeHoverColor = QColor(255, 123, 123);
    QColor m_minPressedColor = QColor(220, 220, 220);
    QColor m_maxPressedColor = QColor(220, 220, 220);
    QColor m_closePressedColor = QColor(255, 103, 103);
};

#endif // TITLEBAR_H
