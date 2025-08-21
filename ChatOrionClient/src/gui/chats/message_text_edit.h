#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMimeType>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QPainter>
#include <QVector>
#include "global.h"

/**
 * @brief 自定义消息输入框
 */
class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget *parent = nullptr);

    ~MessageTextEdit();

    QVector<MsgInfo> getMsgList();

    void insertFileFromUrl(const QStringList &urls);

    void addEmotionUrl(const QString &url);
signals:
    void send();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void insertFromMimeData(const QMimeData *source) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
private:
    void insertImages(const QString &url);
    void insertTextFile(const QString &url);
    void copy();
    void cut();
    void createMimeDataFromSelection(const QTextCursor &cursor, QMimeData *mimeData);
private:
    bool isImage(QString url);//判断文件是否为图片
    void insertMsgList(QVector<MsgInfo> &list,QString flag, QString text, QPixmap pix);

    QStringList getUrl(QString text) const;
    QPixmap getFileIconPixmap(const QString &url);//获取文件图标及大小信息，并转化成图片
    QString getFileSize(qint64 size);//获取文件大小

private slots:
    void textEditChanged();
    void onEmotionImageFrameChange(int frame);
private:
    QVector<MsgInfo> mMsgList;
    QVector<MsgInfo> mGetMsgList;
    QMap<QMovie*, QString> m_emotionMap;
};

#endif // MESSAGETEXTEDIT_H
