#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <QObject>
#include <QNetworkAccessManager>

#include "global.h"
#include "singleton.h"

class HttpMgr : public QObject, public Singleton<HttpMgr>,
        public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    using modules_handlers_t =  std::function<void(ReqId id, QJsonObject res, ErrorCodes err)>;

    ~HttpMgr();

    void postHttpReq(QUrl url, QJsonObject json, Modules mod, ReqId req_id);

    void registerModulesHandlers(Modules mod, modules_handlers_t handlers);
    void unregisterModulesHandlers(Modules mod);
signals:
    void sig_http_finish(Modules mod, ReqId req_id, QString res, ErrorCodes err);
public slots:
    void slot_http_finish(Modules mod, ReqId req_id, QString res, ErrorCodes err);
private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
private:
    QNetworkAccessManager _manager;
    QMap<Modules, modules_handlers_t> _modules_handlers_map;
};

#endif // HTTPMGR_H
