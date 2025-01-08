#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <QCoreApplication>
#include <QCryptographicHash>

class Tools
{
public:
    Tools();
    static QString hashString(const QString& input, QCryptographicHash::Algorithm algorithm);
};

#endif // TOOLS_H
