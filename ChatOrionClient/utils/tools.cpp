#include "tools.h"

#include <keychain.h>
#include <QSettings>
#include <QDateTime>
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

    QObject::connect(job, &QKeychain::Job::finished, job, [](QKeychain::Job* job) {
        if (job->error()) {
            qWarning() << "密码保存失败:" << job->errorString()
                      << " Error code:" << job->error()
                      << " Key:" << job->key();
        } else {
            qDebug() << "密码保存成功:" << job->key();
        }
        job->deleteLater();
    });

    job->start();
}

void Tools::loadPassword(const QString &username, QObject *receiver, std::function<void (const QString &)> callback)
{
    auto* job = new QKeychain::ReadPasswordJob("ChatOrion", receiver);
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
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                          "ChatOrion", "ChatOrion");
    settings.setValue("username", remember ? username : "");
    settings.setValue("rememberPassword", remember);
    settings.setValue("autoLogin", autoLogin);
}

void Tools::loadLoginState(QString &username, bool &remember, bool &autoLogin)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                          "ChatOrion", "ChatOrion");
    username = settings.value("username").toString();
    remember = settings.value("rememberPassword").toBool();
    autoLogin = settings.value("autoLogin").toBool();
}

QString Tools::getFormattedTimeString(const QDateTime &datetime)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDate yesterday = currentDateTime.date().addDays(-1);

    if (datetime.date() == currentDateTime.date()) {
        return datetime.toString("HH:mm");
    } else if (datetime.date() == yesterday) {
        return QString("昨天 ") + datetime.toString("HH:mm");
    } else {
        // 使用自定义格式，只显示年份后两位
        return datetime.toString("yy/MM/dd HH:mm");
    }
}
