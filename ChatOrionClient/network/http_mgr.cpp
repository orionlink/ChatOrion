#include "http_mgr.h"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QMetaObject>   // 后面再包含其他需要的头文件

HttpMgr::HttpMgr()
{
    QObject::connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

HttpMgr::~HttpMgr()
{

}

void HttpMgr::postHttpReq(QUrl url, QJsonObject json, Modules mod, ReqId req_id)
{
   QByteArray data = QJsonDocument(json).toJson();
   QNetworkRequest request(url);
   request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
   request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
   QNetworkReply * reply = _manager.post(request, data);
   auto self = shared_from_this();
   QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod]()
   {
       if (reply->error() != QNetworkReply::NoError)
       {
           qDebug() << reply->errorString();
           emit self->sig_http_finish(mod, req_id, "", ErrorCodes::ERR_NETWORK);
           reply->deleteLater();
           return;
       }

       QString res = reply->readAll();
       emit self->sig_http_finish(mod, req_id, res, ErrorCodes::SUCCESS);
       reply->deleteLater();
   });
}

void HttpMgr::registerModulesHandlers(Modules mod, HttpMgr::modules_handlers_t handlers)
{
    _modules_handlers_map.insert(mod, handlers);
}

void HttpMgr::unregisterModulesHandlers(Modules mod)
{
    _modules_handlers_map.remove(mod);
}

void HttpMgr::slot_http_finish(Modules mod, ReqId id, QString res, ErrorCodes err)
{
    QJsonDocument json_doc = QJsonDocument::fromJson(res.toUtf8());
    if (json_doc.isEmpty() || !json_doc.isObject())
    {
        err = ERR_JSON;
        qDebug() << "Json 解析失败或者为空";
    }

    QJsonObject json_obj = json_doc.object();

    _modules_handlers_map[mod](id, json_obj, err);
}
