#include "tools.h"

#include <qt5keychain/keychain.h>
#include <QSettings>
#include <QDebug>

Tools::Tools()
{
}

QString Tools::hashString(const QString &input, QCryptographicHash::Algorithm algorithm)
{
    QByteArray data = input.toUtf8();

    QCryptographicHash hash(algorithm);

    hash.addData(data);

    QByteArray result = hash.result();

    return QString(result.toHex());
}

void Tools::savePassword(const QString &username, const QString &password)
{
    auto* job = new QKeychain::WritePasswordJob("ChatOrion");
    job->setKey(username);
    job->setTextData(password);
    job->start();
}

void Tools::loadPassword(const QString &username, QObject *receiver, std::function<void (const QString &)> callback)
{
    auto* job = new QKeychain::ReadPasswordJob("ChatOrion");
    job->setKey(username);
    QObject::connect(job, &QKeychain::Job::finished, receiver, [callback](QKeychain::Job* job)
    {
        if (job->error())
        {
            qWarning() << "密码读取失败:" << job->errorString();
            callback("");
        } else
        {
            auto* readJob = static_cast<QKeychain::ReadPasswordJob*>(job);
            callback(readJob->textData());
        }
        job->deleteLater();
    });
    job->start();
}

void Tools::saveLoginState(const QString &username, bool remember, bool autoLogin)
{
    QSettings settings;
    settings.setValue("username", remember ? username : "");
    settings.setValue("rememberPassword", remember);
    settings.setValue("autoLogin", autoLogin);
}

void Tools::loadLoginState(QString &username, bool &remember, bool &autoLogin)
{
    QSettings settings;
    username = settings.value("username").toString();
    remember = settings.value("rememberPassword").toBool();
    autoLogin = settings.value("autoLogin").toBool();
}
