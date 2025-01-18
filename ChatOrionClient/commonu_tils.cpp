#include "common_utils.h"

#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QSettings>
#include <QApplication>
#include <QColor>

JsonHandler::JsonHandler(QObject *parent) : QObject(parent)
{
}

bool JsonHandler::readJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qWarning() << "Failed to create JSON document.";
        return false;
    }

    // 判断是对象还是数组
    if (jsonDoc.isObject()) {
        m_jsonObject = jsonDoc.object();
        m_jsonArray = QJsonArray(); // 清空数组
    } else if (jsonDoc.isArray()) {
        m_jsonArray = jsonDoc.array();
        m_jsonObject = QJsonObject(); // 清空对象
    } else {
        qWarning() << "JSON document is neither an object nor an array.";
        return false;
    }

    return true;
}

bool JsonHandler::writeJsonFile(const QString &filePath, const QJsonObject &jsonObject)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QJsonDocument jsonDoc(jsonObject);
    file.write(jsonDoc.toJson());
    file.close();

    return true;
}

bool JsonHandler::writeJsonFile(const QString &filePath, const QJsonArray &jsonArray)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QJsonDocument jsonDoc(jsonArray);
    file.write(jsonDoc.toJson());
    file.close();

    return true;
}

QJsonObject JsonHandler::getJsonObject() const
{
    return m_jsonObject;
}

void JsonHandler::setJsonObject(const QJsonObject &jsonObject)
{
    m_jsonObject = jsonObject;
    m_jsonArray = QJsonArray(); // 清空数组
}

QJsonArray JsonHandler::getJsonArray() const
{
    return m_jsonArray;
}

void JsonHandler::setJsonArray(const QJsonArray &jsonArray)
{
    m_jsonArray = jsonArray;
    m_jsonObject = QJsonObject(); // 清空对象
}

CommonUtils::CommonUtils()
{

}

QPixmap CommonUtils::getRoundImage(const QPixmap &src, QPixmap& mask, QSize masksize)
{
	if (masksize == QSize(0, 0))
	{
		masksize = mask.size();
	}
	else
	{
		mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	QImage resultImage(masksize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImage);
}

void CommonUtils::loadStyleSheet(QWidget* widget, const QString &sheetName)
{
    QFile file(":/res/qss/" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	if (file.isOpen())
	{
		widget->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(qsstyleSheet);
	}
	file.close();
}

QString CommonUtils::CharacterEscaping(QString content)
{
	QString result;
	result = content.replace("\"", "&quot;");
	result = content.replace("&", "&amp;");
	result = content.replace("<", "&lt;");
	result = content.replace(">", "&gt;");
	result = content.replace("\\", "&#92;");
	return result;
}

void CommonUtils::setDefaultSkinColor(const QColor& color)
{
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	QSettings settings(path, QSettings::IniFormat);
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());
}

QColor CommonUtils::getDefaultSkinColor()
{
	QColor color;
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	if (!QFile::exists(path))
		setDefaultSkinColor(QColor(22, 154, 218));
	QSettings settings(path, QSettings::IniFormat);
	color.setRed(settings.value("DefaultSkin/red").toInt());
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());
	return color;
}

