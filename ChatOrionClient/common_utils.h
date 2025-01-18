#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <QSize>
#include <QPixmap>
#include <QProxyStyle>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QObject>

class JsonHandler : public QObject
{
    Q_OBJECT

public:
    explicit JsonHandler(QObject *parent = nullptr);
    ~JsonHandler() {}

    // 读取JSON文件
    bool readJsonFile(const QString &filePath);

    // 写入JSON文件
    bool writeJsonFile(const QString &filePath, const QJsonObject &jsonObject);
    bool writeJsonFile(const QString &filePath, const QJsonArray &jsonArray);

    // 获取JSON对象
    QJsonObject getJsonObject() const;

    // 设置JSON对象
    void setJsonObject(const QJsonObject &jsonObject);

    // 获取JSON数组
    QJsonArray getJsonArray() const;

    // 设置JSON数组
    void setJsonArray(const QJsonArray &jsonArray);

private:
    QJsonObject m_jsonObject;
    QJsonArray m_jsonArray;
};

class CustomProxyStyle : public QProxyStyle
{
public:
	CustomProxyStyle(QObject* parent)
	{
		setParent(parent);
	};

	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget = 0) const
	{
		if (PE_FrameFocusRect == element)
		{
			return;
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

class CommonUtils
{
public:
	CommonUtils();

public:
	static QPixmap getRoundImage(const QPixmap &src, QPixmap& mask, QSize masksize = QSize(0, 0));
	static void loadStyleSheet(QWidget* widget, const QString &sheetName);
	static QString CharacterEscaping(QString content);
	static void setDefaultSkinColor(const QColor& color);
	static QColor getDefaultSkinColor();
};

#endif // COMMONUTILS_H
