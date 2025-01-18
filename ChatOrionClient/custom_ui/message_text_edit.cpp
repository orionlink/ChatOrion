#include "message_text_edit.h"
#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <QDir>
#include <QUuid>
#include <QTextCursor>
#include <QTextBlock>
#include <QClipboard>
#include <QMenu>
#include <QAction>

MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent)
{

    //this->setStyleSheet("border: none;");
    this->setMaximumHeight(60);

//    connect(this,SIGNAL(textChanged()),this,SLOT(textEditChanged()));

    QAction *copyAction = new QAction(tr("复制"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MessageTextEdit::copy);
    addAction(copyAction);

    QAction *cutAction = new QAction(tr("剪切"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MessageTextEdit::cut);
    addAction(cutAction);

    // 移除默认的快捷键动作
    QList<QAction*> actions = this->actions();
    for (QAction* action : actions) {
        if (action->shortcuts().contains(QKeySequence::Copy) ||
            action->shortcuts().contains(QKeySequence::Cut) ||
            action->shortcuts().contains(QKeySequence::Paste))
        {
            this->removeAction(action);
        }
    }
}

MessageTextEdit::~MessageTextEdit()
{

}

QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();

    QString doc = this->document()->toPlainText();
    QString text="";//存储文本信息
    int indexUrl = 0;
    int count = mMsgList.size();

    for(int index=0; index<doc.size(); index++)
    {
        if(doc[index]==QChar::ObjectReplacementCharacter)
        {
            if(!text.isEmpty())
            {
                QPixmap pix;
                insertMsgList(mGetMsgList,"text",text,pix);
                text.clear();
            }
            while(indexUrl<count)
            {
                MsgInfo msg =  mMsgList[indexUrl];
                if(this->document()->toHtml().contains(msg.content,Qt::CaseSensitive))
                {
                    indexUrl++;
                    mGetMsgList.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else
        {
            text.append(doc[index]);
        }
    }
    if(!text.isEmpty())
    {
        QPixmap pix;
        insertMsgList(mGetMsgList,"text",text,pix);
        text.clear();
    }
    mMsgList.clear();
    this->clear();
    return mGetMsgList;
}

void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source()==this)
        event->ignore();
    else
        event->accept();
}

void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) && !(e->modifiers() & Qt::ShiftModifier)) {
        emit send();
        return;
    }

    if (e->modifiers() & Qt::ControlModifier) {
        switch (e->key()) {
            case Qt::Key_C:
                if (textCursor().hasSelection()) {
                    copy();
                }
                return;

            case Qt::Key_V:
                if (const QMimeData *md = QGuiApplication::clipboard()->mimeData()) {
                    insertFromMimeData(md);
                }
                return;
        }
    }

    QTextEdit::keyPressEvent(e);
}

void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if(urls.isEmpty())
        return;

    foreach (QString url, urls){
         if(isImage(url))
             insertImages(url);
         else
             insertTextFile(url);
    }
}

void MessageTextEdit::addEmotionUrl(const QString &url)
{
    insertHtml(QString("<img src='%1' width='32' height='32'/>").arg(url));

    QMovie* apngMovie = new QMovie(url, "apng", this);
    apngMovie->setCacheMode(QMovie::CacheNone);
    apngMovie->setScaledSize(QSize(32, 32));
    if (!apngMovie->isValid()) {
        qWarning() << "Failed to load APNG:" << url;
        return;
    }

    connect(apngMovie, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));
    apngMovie->start();

    m_emotionMap.insert(apngMovie, url);

    // 将表情消息添加到消息列表
    insertMsgList(mMsgList, "emotion", url, QPixmap());  // 表情消息类型为 "emotion"

    updateGeometry();
}

void MessageTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    //按比例缩放图片
    if(image.width()>120||image.height()>80)
    {
        if(image.width()>image.height())
        {
          image =  image.scaledToWidth(120,Qt::SmoothTransformation);
        }
        else
            image = image.scaledToHeight(80,Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();
    // QTextDocument *document = this->document();
    // document->addResource(QTextDocument::ImageResource, QUrl(url), QVariant(image));
    cursor.insertImage(image,url);

    insertMsgList(mMsgList,"image",url,QPixmap::fromImage(image));
}

void MessageTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    if(fileInfo.isDir())
    {
        QMessageBox::information(this,"提示","只允许拖拽单个文件!");
        return;
    }

    if(fileInfo.size()>100*1024*1024)
    {
        QMessageBox::information(this,"提示","发送的文件大小不能大于100M");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(),url);
    insertMsgList(mMsgList,"file",url,pix);
}

void MessageTextEdit::createMimeData(const QTextCursor &cursor, QMimeData *mimeData)
{
    // 获取选中的文本片段
    QString plainText;
    QString html = "<html><body>";
    QVector<MsgInfo> messages;

    QTextDocument *doc = document();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QTextBlock block = doc->findBlock(start);
    QTextBlock endBlock = doc->findBlock(end);

    while (block.isValid() && block.blockNumber() <= endBlock.blockNumber()) {
        QTextBlock::iterator it;
        for (it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;

            int fragStart = fragment.position();
            int fragEnd = fragStart + fragment.length();

            if (fragEnd <= start || fragStart >= end)
                continue;

            QTextCharFormat format = fragment.charFormat();
            if (format.objectType() == QTextFormat::NoObject) {
                // 普通文本
                QString text = fragment.text();
                plainText += text;
                html += text.toHtmlEscaped();

                MsgInfo msgInfo;
                msgInfo.msgFlag = "text";
                msgInfo.content = text;
                messages.append(msgInfo);
            }
            else if (format.isImageFormat()) {
                QTextImageFormat imgFormat = format.toImageFormat();
                QString imgSource = imgFormat.name();

                bool isEmoticon = false;
                for (const MsgInfo &msg : mMsgList) {
                    if (msg.msgFlag == "emotion" && msg.content == imgSource) {
                        isEmoticon = true;
                        break;
                    }
                }

                if (isEmoticon) {
                    html += QString("<img class='emotion' src='%1' width='32' height='32'/>").arg(imgSource);

                    MsgInfo msgInfo;
                    msgInfo.msgFlag = "emotion";
                    msgInfo.content = imgSource;
                    messages.append(msgInfo);
                }
            }
        }
        block = block.next();
    }

    html += "</body></html>";

    // 设置 MIME 数据
    mimeData->setText(plainText);
    mimeData->setHtml(html);

    // 添加自定义消息格式
    QByteArray messageData;
    QDataStream stream(&messageData, QIODevice::WriteOnly);
    stream << messages;
    mimeData->setData("application/x-chat-messages", messageData);
}

void MessageTextEdit::copy()
{
//    QTextCursor cursor = textCursor();
//    if (!cursor.hasSelection())
//        return;

//    QMimeData *mimeData = new QMimeData;
//    createMimeData(cursor, mimeData);
//    QClipboard *clipboard = QGuiApplication::clipboard();
//    clipboard->setMimeData(mimeData);

    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection()) return;

    QMimeData *mimeData = new QMimeData;
    createMimeDataFromSelection(cursor, mimeData);
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void MessageTextEdit::cut()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        return;

    QMimeData *mimeData = new QMimeData;
    createMimeData(cursor, mimeData);
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);

    cursor.removeSelectedText();
}

void MessageTextEdit::createMimeDataFromSelection(const QTextCursor &cursor, QMimeData *mimeData)
{
    if (!cursor.hasSelection()) return;

    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QString plainText;
    QString html = "<html><body>";
    QVector<MsgInfo> messages;

    QTextDocument* doc = document();
    QTextBlock block = doc->findBlock(start);
    QTextBlock endBlock = doc->findBlock(end);

    while (block.isValid() && block.blockNumber() <= endBlock.blockNumber()) {
        int blockStart = block.position();

        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid()) continue;

            int fragStart = fragment.position();
            int fragEnd = fragStart + fragment.length();

            // 检查片段是否在选中范围内
            if (fragEnd <= start || fragStart >= end) continue;

            // 计算实际要复制的文本范围
            int copyStart = qMax(fragStart, start) - fragStart;
            int copyEnd = qMin(fragEnd, end) - fragStart;
            int copyLength = copyEnd - copyStart;

            QTextCharFormat format = fragment.charFormat();
            if (format.objectType() == QTextFormat::NoObject) {
                // 文本内容
                QString text = fragment.text().mid(copyStart, copyLength);
                plainText += text;
                html += text.toHtmlEscaped();

                MsgInfo msgInfo;
                msgInfo.msgFlag = "text";
                msgInfo.content = text;
                messages.append(msgInfo);
            }
            else if (format.isImageFormat()) {
                QTextImageFormat imgFormat = format.toImageFormat();
                QString imgSource = imgFormat.name();

                // 检查是否是表情，且表情完全在选中范围内
                if (fragStart >= start && fragEnd <= end) {
                    bool isEmoticon = false;
                    for (const MsgInfo &msg : mMsgList) {
                        if (msg.msgFlag == "emotion" && msg.content == imgSource) {
                            isEmoticon = true;
                            break;
                        }
                    }

                    if (isEmoticon) {
                        html += QString("<img class='emotion' src='%1' width='32' height='32'/>").arg(imgSource);

                        MsgInfo msgInfo;
                        msgInfo.msgFlag = "emotion";
                        msgInfo.content = imgSource;
                        messages.append(msgInfo);
                    }
                }
            }
        }
        block = block.next();
    }

    html += "</body></html>";

    mimeData->setText(plainText);
    mimeData->setHtml(html);

    QByteArray messageData;
    QDataStream stream(&messageData, QIODevice::WriteOnly);
    stream << messages;
    mimeData->setData("application/x-chat-messages", messageData);
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    if (source->hasImage())
        return true;
    if (source->hasHtml())
        return true;
    if (source->hasText())
        return true;
    if (!getUrl(source->text()).isEmpty())
        return true;

    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{

    // 首先检查是否是自定义消息格式
   if (source->hasFormat("application/x-chat-messages"))
   {
       QByteArray messageData = source->data("application/x-chat-messages");
       QDataStream stream(messageData);
       QVector<MsgInfo> messages;
       stream >> messages;

       // 插入所有消息
       for (const MsgInfo& msg : messages)
       {
           if (msg.msgFlag == "text")
           {
               insertPlainText(msg.content);
           }
           else if (msg.msgFlag == "emotion")
           {
               addEmotionUrl(msg.content);
           }
       }
       return;
   }

   // 如果是HTML格式且包含表情
   if (source->hasHtml())
   {
       QString html = source->html();
       QTextDocument tempDoc;
       tempDoc.setHtml(html);

       QTextCursor tempCursor(&tempDoc);
       tempCursor.movePosition(QTextCursor::Start);

       while (!tempCursor.atEnd())
       {
           QTextCharFormat format = tempCursor.charFormat();
           if (tempCursor.charFormat().isImageFormat())
           {
               QTextImageFormat imgFormat = tempCursor.charFormat().toImageFormat();
               QString imgSource = imgFormat.name();

               if (html.contains(QString("<img[^>]*class=[\"\']emotion[\"\'][^>]*src=[\"\']%1[\"\']").arg(imgSource))) {
                   addEmotionUrl(imgSource);
               }
               else
               {
                   // 处理其他图片
                   QTextEdit::insertFromMimeData(source);
               }
           }
           else
           {
               // 普通文本
               QTextCursor cursor = textCursor();
               cursor.insertText(tempDoc.characterAt(tempCursor.position()));
           }
           tempCursor.movePosition(QTextCursor::NextCharacter);
       }
       return;
   }

    // 处理图片类型
    if (source->hasImage())
    {
        QImage image = qvariant_cast<QImage>(source->imageData());
        // 按比例缩放图片
        if(image.width() > 120 || image.height() > 80) {
            if(image.width() > image.height()) {
                image = image.scaledToWidth(120, Qt::SmoothTransformation);
            } else {
                image = image.scaledToHeight(80, Qt::SmoothTransformation);
            }
        }

        // 生成临时文件名用于保存图片
        QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString() + ".png";
        image.save(tempPath, "PNG");

        QTextCursor cursor = this->textCursor();
        cursor.insertImage(image, tempPath);
        insertMsgList(mMsgList, "image", tempPath, QPixmap::fromImage(image));
        return;
    }

    // 处理纯文本
    if (source->hasText())
    {
        QTextEdit::insertPlainText(source->text());
        return;
    }

    // 处理其他类型的内容
    QTextEdit::insertFromMimeData(source);
}

void MessageTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    // 移除原有的复制/剪切动作
    QList<QAction*> actions = menu->actions();
    for(QAction* action : actions) {
        if(action->text().contains("复制") || action->text().contains("Copy") ||
           action->text().contains("剪切") || action->text().contains("Cut")) {
            menu->removeAction(action);
        }
    }

    // 添加自定义的复制/剪切动作
    QAction *copyAction = new QAction(tr("复制"), menu);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MessageTextEdit::copy);

    QAction *cutAction = new QAction(tr("剪切"), menu);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MessageTextEdit::cut);

    menu->insertAction(actions.first(), copyAction);
    menu->insertAction(copyAction, cutAction);

    menu->exec(event->globalPos());
    delete menu;
}

bool MessageTextEdit::isImage(QString url)
{
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pix)
{
    MsgInfo msg;
    msg.msgFlag=flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}

QStringList MessageTextEdit::getUrl(QString text) const
{
    QStringList urls;
    if(text.isEmpty()) return urls;

    QStringList list = text.split("\n");
    foreach (QString url, list) {
        if(!url.isEmpty()){
            QStringList str = url.split("///");
            if(str.size()>=2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
   // painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

void MessageTextEdit::textEditChanged()
{
    //qDebug() << "text changed!" << endl;
}

void MessageTextEdit::onEmotionImageFrameChange(int frame)
{
    Q_UNUSED(frame);
    QMovie* movie = qobject_cast<QMovie*>(sender());
    document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
    setLineWrapColumnOrWidth(lineWrapColumnOrWidth());
}
