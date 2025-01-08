#include "tools.h"

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
