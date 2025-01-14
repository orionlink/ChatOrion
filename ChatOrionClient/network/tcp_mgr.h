#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>

#include "singleton.h"
#include "global.h"

class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    friend class Singleton<TcpMgr>;
    using message_handle_t = std::function<void(int len, QByteArray data)>;

    ~TcpMgr();

    void registerMessageCallback(ReqId id, message_handle_t handle);
    void unregisterMessageCallback(ReqId id);
private:
    TcpMgr();
    void handleMsg(ReqId id, int len, QByteArray data);

    QTcpSocket _socket;
    QByteArray _buffer;
    QString _host;
    uint16_t _port;
    bool _b_recv_pending; // 没有读完则为true
    quint16 _message_id;  // 消息id
    quint16 _message_len; // 消息长度
    QMap<ReqId, message_handle_t> _handlers;
public slots:
    void slot_tcp_connect(const QString& host, uint16_t port);
    void slot_send_data(ReqId reqId, QByteArray dataBytes);
signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QByteArray data);
    void sig_swich_chatdlg();
};

#endif // TCPMGR_H
