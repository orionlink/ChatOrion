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

    static void savePassword(const QString& username, const QString& password);
    static void loadPassword(const QString& username, QObject* receiver, std::function<void(const QString&)> callback);

    static void saveLoginState(const QString& username, bool remember, bool autoLogin);
    static void loadLoginState(QString& username, bool& remember, bool& autoLogin);
};

#endif // TOOLS_H
