#include "content_bubble.h"
#include "chat_view.h"
#include "cui_helper.h"
#include "user_data.h"

#include <QDebug>
#include <QRegularExpression>

ContentBubbleFrame::ContentBubbleFrame(ChatRole role, QWidget *parent)
    : BubbleFrame(role, parent)
    , m_contentLabel(new ContentLabel(this))
{
    m_contentLabel->setStyleSheet("QTextEdit { background: transparent; }");
    setWidget(m_contentLabel);
    m_contentLabel->installEventFilter(this);
}

bool ContentBubbleFrame::setContent(const QVector<MsgInfo> &messages)
{
     m_messages = messages; // 存储消息

    // 清理旧的动画
    qDeleteAll(m_emotionMovies);
    m_emotionMovies.clear();

    QTextCursor cursor(m_contentLabel->document());
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    for (const MsgInfo& msg : messages) {
        if (msg.msgFlag == "text") {
            cursor.insertText(msg.content);
        }
        else if (msg.msgFlag == "emotion") {
            insertEmotion(cursor, msg.content);
        }
    }

    cursor.endEditBlock();

    calculateMaxWidth(messages);

    updateGeometry();

    return !cursor.document()->toPlainText().isEmpty();
}

bool ContentBubbleFrame::setContent(const QString &messages)
{
    QVector<MsgInfo> msgInfo = parseMixedMessage(messages);

    return setContent(msgInfo);
}

void ContentBubbleFrame::contextMenuEvent(QContextMenuEvent *event)
{
    // 如果有文本被选中，则显示复制菜单
    if (textIsSelected())
    {
        QMenu menu(this);
        CUIHelper::GetInstance()->setMenuRadius(&menu);
        QAction* copyAction = m_contentLabel->createCopyAction();
        copyAction->setEnabled(true);
        menu.addAction(copyAction);
        menu.exec(event->globalPos());
        event->accept();
        return;
    }

    // 其他情况传递给基类处理
    BubbleFrame::contextMenuEvent(event);
}

bool ContentBubbleFrame::textIsSelected() const
{
     return m_contentLabel && m_contentLabel->textCursor().hasSelection();
}

void ContentBubbleFrame::insertEmotion(QTextCursor &cursor, const QString &emotionPath)
{
    QMovie* movie = new QMovie(emotionPath, "apng", this);
    movie->setCacheMode(QMovie::CacheNone);
    movie->setScaledSize(QSize(32, 32));

    if (movie->isValid())
    {
        m_emotionMovies[emotionPath] = movie;

        // 创建表情字符格式
        QTextCharFormat emotionFormat;
        emotionFormat.setObjectType(EmotionInterface::EmotionType);
        emotionFormat.setProperty(EmotionInterface::EmotionProperty, emotionPath);

        // 连接动画帧变化信号
        QObject::connect(movie, &QMovie::frameChanged, this, [this, emotionPath, movie]()
        {
            m_contentLabel->emotionInterface()->updateEmotionPixmap(
                emotionPath,
                movie->currentPixmap()
            );
            m_contentLabel->viewport()->update();
        });

        cursor.insertText(QString(QChar::ObjectReplacementCharacter), emotionFormat);
        movie->start();
    }
}

void ContentBubbleFrame::adjustTextHeight()
{
    //字体到边框的距离默认为4
    qreal doc_margin = m_contentLabel->document()->documentMargin();
    QTextDocument *doc = m_contentLabel->document();
    qreal text_height = 0;
    //把每一行的高度相加=文本高
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextLayout *pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();                             //这段的rect
        text_height += text_rect.height();
    }
    // 布局器的间隙, 上下各一个，共2，所以 * 2
    int vMargin = this->layout()->contentsMargins().top();
    //设置这个气泡需要的高度 文本高+文本边距+TextEdit边框到气泡边框的距离
    // 是一个矩形，改变高度，宽度也顺着改变
    setFixedHeight(text_height + doc_margin *2 + vMargin*2);
}

bool ContentBubbleFrame::eventFilter(QObject *o, QEvent *e)
{
    if(m_contentLabel == o && e->type() == QEvent::Paint) // 窗口大小改变，气泡改变高度
    {
        adjustTextHeight(); //PaintEvent中设置
    }
    return BubbleFrame::eventFilter(o, e);
}

void ContentBubbleFrame::calculateMaxWidth(const QVector<MsgInfo> &messages)
{
    qreal doc_margin = m_contentLabel->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(m_contentLabel->font());
    QTextDocument *doc = m_contentLabel->document();
    int max_width = 0;
    //遍历每一段找到 最宽的那一段
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
    {
        int txtW = int(fm.width(it.text()));
        max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
    }

    int currentLineWidth = 0;
    for (const MsgInfo& msg : messages)
    {
        if (msg.msgFlag == "emotion") {
            // 表情固定宽度 32px + 间距
            currentLineWidth += 32 + 2; // 2px作为表情间距
        }
    }

    //设置这个气泡的最大宽度 只需要设置一次
    // 增加一个缓冲区（例如 10 像素），确保文本不会因为计算误差而换行
    int buffer = 5;
    setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right) + buffer + currentLineWidth);//设置最大宽度
}

QVector<MsgInfo> ContentBubbleFrame::parseMixedMessage(const QString &message)
{
    QVector<MsgInfo> result;

    // 正则表达式匹配规则：匹配文本块或表情块（例如 [戳一戳]）
    QRegularExpression regex(R"((\[.*?\])|([^[]+))"); // 非贪婪匹配表情，剩余部分为文本
    QRegularExpressionMatchIterator iter = regex.globalMatch(message);

    while (iter.hasNext())
    {
        QRegularExpressionMatch match = iter.next();
        QString emotionPart = match.captured(1); // 捕获表情部分（包含方括号）
        QString textPart = match.captured(2);    // 捕获文本部分

        if (!emotionPart.isEmpty()) {
            // 处理表情：去除方括号
            QString key = emotionPart.mid(1, emotionPart.length() - 2); // 去掉首尾的[]
            QJsonObject m_emojiObject = EmojiManager::GetInstance()->getEmojiObject();
            if (!m_emojiObject.isEmpty())
            {
                QString content = m_emojiObject.value(key).toString();
                result.append({"emotion", content});
            }
        } else if (!textPart.isEmpty()) {
            // 处理文本
            result.append({"text", textPart});
        }
    }

    return result;
}

ContentLabel::ContentLabel(QWidget *parent) : QTextEdit(parent)
{
    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    this->setFont(font);

    setReadOnly(true);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    document()->setDocumentMargin(0);
    setStyleSheet("QTextEdit { background: transparent; color: black; }"
                 "QTextEdit::selection { background: #b5d5ff; }");

    // 设置文本交互标志，只允许选择，不显示光标
    setTextInteractionFlags(Qt::TextSelectableByMouse| Qt::TextSelectableByKeyboard);

    // 禁用光标闪烁
    setCursorWidth(0);

    // 安装表情接口
    m_emotionInterface = new EmotionInterface(this);
    document()->documentLayout()->registerHandler(EmotionInterface::EmotionType, m_emotionInterface);

    // 添加复制功能的快捷键
    addAction(createCopyAction());

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

void ContentLabel::focusInEvent(QFocusEvent *e)
{
    QTextEdit::focusInEvent(e);
    setCursorWidth(0); // 确保光标始终不可见
}

void ContentLabel::mousePressEvent(QMouseEvent *e)
{
    // 先检查是否在多选模式
    ChatView* chatView = qobject_cast<ChatView*>(window()->findChild<ChatView*>());
    if (chatView && chatView->isInSelectionMode()) {
        e->ignore();  // 忽略事件，让它继续传递
        return;
    }

    // 不是多选模式时正常处理
    QTextEdit::mousePressEvent(e);
    setCursorWidth(0);
}

void ContentLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextEdit::mouseDoubleClickEvent(e);
    setCursorWidth(0);
}

void ContentLabel::contextMenuEvent(QContextMenuEvent *event)
{
//    QMenu *menu = new QMenu(this);

//    // 只添加复制选项
//    QAction* copyAction = createCopyAction();
//    copyAction->setEnabled(textCursor().hasSelection());
//    menu->addAction(copyAction);

//    menu->exec(event->globalPos());
//    delete menu;

    event->ignore();  // 让事件继续传播
}

void ContentLabel::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_C)
    {
        qDebug() << "Ctrl+C detected";
        if (textCursor().hasSelection())
        {
            copySelectedContent();
            return;
        }
    }

    QTextEdit::keyPressEvent(e); // 调用基类处理其他事件
}

void ContentLabel::copySelectedContent()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection()) return;

    QMimeData *mimeData = new QMimeData;
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QString plainText;
    QString html = "<html><body>";
    QVector<MsgInfo> messages;

    QTextDocument* doc = document();
    QTextBlock block = doc->findBlock(start);
    QTextBlock endBlock = doc->findBlock(end);

    while (block.isValid() && block.blockNumber() <= endBlock.blockNumber()) {
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

            if (fragment.charFormat().objectType() == EmotionInterface::EmotionType) {
                // 表情内容 - 只有当表情完全在选中范围内时才复制
                if (fragStart >= start && fragEnd <= end) {
                    QTextCharFormat format = fragment.charFormat();
                    QString emotionUrl = format.property(EmotionInterface::EmotionProperty).toString();

                    html += QString("<img class='emotion' src='%1' width='32' height='32'/>").arg(emotionUrl);

                    MsgInfo msgInfo;
                    msgInfo.msgFlag = "emotion";
                    msgInfo.content = emotionUrl;
                    messages.append(msgInfo);
                }
            } else {
                // 文本内容
                QString text = fragment.text().mid(copyStart, copyLength);
                plainText += text;
                html += text.toHtmlEscaped();

                MsgInfo msgInfo;
                msgInfo.msgFlag = "text";
                msgInfo.content = text;
                messages.append(msgInfo);
            }
        }
        block = block.next();
    }

    html += "</body></html>";

    mimeData->setText(plainText);
    mimeData->setHtml(html);

    // 添加自定义消息格式
    QByteArray messageData;
    QDataStream stream(&messageData, QIODevice::WriteOnly);
    stream << messages;
    mimeData->setData("application/x-chat-messages", messageData);

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

QAction *ContentLabel::createCopyAction()
{
    QAction* copyAction = new QAction(tr("复制"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &ContentLabel::copySelectedContent);
    return copyAction;
}

