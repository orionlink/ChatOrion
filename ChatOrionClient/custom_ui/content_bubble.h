#ifndef CONTENTBUBBLEFRAME_H
#define CONTENTBUBBLEFRAME_H

#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMovie>
#include <QMap>
#include <QTextDocument>
#include <QTextObjectInterface>
#include <QPainter>
#include <QTextBlock>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QMenu>

#include "bubble_frame.h"

class ContentLabel;
class EmotionInterface;

/**
 * @brief 复合气泡，支持表情和文本混合，增加复制功能
 */
class ContentBubbleFrame : public BubbleFrame
{
    Q_OBJECT
public:
    explicit ContentBubbleFrame(ChatRole role, QWidget *parent = nullptr);
    ~ContentBubbleFrame()
    {
        qDeleteAll(m_emotionMovies);
    }
    bool setContent(const QVector<MsgInfo>& messages);

    bool setContent(const QString& messages);

    // 获取当前气泡中的所有消息
    QVector<MsgInfo> getMessages() const
    {
        return m_messages;
    }
protected:
    /**
     * @brief 创建菜单项
     * @param event
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

    bool eventFilter(QObject *o, QEvent *e) override;

    bool textIsSelected() const;
private:
    void insertEmotion(QTextCursor &cursor, const QString &emotionPath);
    void adjustTextHeight();
    void calculateMaxWidth(const QVector<MsgInfo>& messages);
    QVector<MsgInfo> parseMixedMessage(const QString& message);

    ContentLabel* m_contentLabel;
    QMap<QString, QMovie*> m_emotionMovies;
    QVector<MsgInfo> m_messages; // 存储当前气泡的所有消息
};

/**
 * @brief 自定义文本对象接口来处理表情动画
 */
class EmotionInterface : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    EmotionInterface(QObject *parent = nullptr) : QObject(parent) {}

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                        const QTextFormat &format) override
    {
        Q_UNUSED(doc);
        Q_UNUSED(posInDocument);
        Q_UNUSED(format);
        return QSizeF(32, 32);  // 表情固定大小
    }

    void drawObject(QPainter *painter, const QRectF &rect,
                   QTextDocument *doc, int posInDocument,
                   const QTextFormat &format) override
    {
        Q_UNUSED(doc);
        Q_UNUSED(posInDocument);

        QString emotionUrl = format.property(EmotionProperty).toString();
        if (m_emotionPixmaps.contains(emotionUrl)) {
            painter->drawPixmap(rect.toRect(), m_emotionPixmaps[emotionUrl]);
        }
    }

    void updateEmotionPixmap(const QString &url, const QPixmap &pixmap)
    {
        m_emotionPixmaps[url] = pixmap;
    }

    static const int EmotionType = QTextFormat::UserObject + 1;
    static const int EmotionProperty = QTextFormat::UserProperty + 1;

private:
    QMap<QString, QPixmap> m_emotionPixmaps;
};

class ContentLabel : public QTextEdit
{
    Q_OBJECT
public:
    explicit ContentLabel(QWidget *parent = nullptr);

    EmotionInterface* emotionInterface() const { return m_emotionInterface; }

    QAction* createCopyAction();
protected:
    void focusInEvent(QFocusEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;

    void mouseDoubleClickEvent(QMouseEvent *e) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

    void keyPressEvent(QKeyEvent *e) override;
private slots:
    void copySelectedContent();

private:

    EmotionInterface* m_emotionInterface;
};

#endif // CONTENTBUBBLEFRAME_H
