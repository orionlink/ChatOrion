#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>

/**
 * @brief 圆角对话框，可拖动
 */
class BaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BaseDialog(QWidget *parent = nullptr);
    ~BaseDialog() = default;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
private:
    bool _isPressed = false;
    QPoint _dragPosition;
};

#endif // BASEDIALOG_H
