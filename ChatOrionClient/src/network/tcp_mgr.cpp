#include "tcp_mgr.h"

#include <QAbstractSocket>
#include <QtCore/QtEndian>

TcpMgr::TcpMgr() : _host(""), _port(0),_b_recv_pending(false),_message_id(0),_message_len(0)
{
    QObject::connect(&_socket, &QTcpSocket::connected, [this]{
         qDebug() << "服务器连接成功";
         // 连接建立后发送消息
         emit sig_con_success(true);
    });

    QObject::connect(&_socket, &QTcpSocket::readyRead, [&]()
    {
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        _buffer.append(_socket.readAll());

        forever {
            // 先解析头部
            if (!_b_recv_pending) {
                // 检查缓冲区中的数据是否足够解析出一个消息头
                if (_buffer.size() < static_cast<int>(sizeof(qint16) * 2)) {
                    return;
                }

                // 使用指针直接读取网络字节序的数据
                const char* data = _buffer.constData();
                qint16 msgId, msgLen;

                // 读取消息ID并转换字节序
                memcpy(&msgId, data, sizeof(qint16));
                _message_id = qFromBigEndian(msgId);  // 或者用 ntohs(msgId)

                // 读取消息长度并转换字节序
                memcpy(&msgLen, data + sizeof(qint16), sizeof(qint16));
                _message_len = qFromBigEndian(msgLen);  // 或者用 ntohs(msgLen)

                // 移除已处理的头部数据
                _buffer = _buffer.mid(sizeof(qint16) * 2);

                qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
            }

            // 后续代码保持不变
            if (_buffer.size() < _message_len) {
                _b_recv_pending = true;
                return;
            }

            _b_recv_pending = false;

            QByteArray messageBody = _buffer.mid(0, _message_len);
            qDebug() << "receive body msg is " << messageBody;

            _buffer = _buffer.mid(_message_len);

            handleMsg(ReqId(_message_id), _message_len, messageBody);
        }
    });

    // 处理错误（适用于Qt 5.15之前的版本）
    QObject::connect(&_socket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
                        [&](QTcpSocket::SocketError socketError) {
           QString error_str = _socket.errorString();
           qDebug() << "Error:" << error_str;
           switch (socketError) {
               case QTcpSocket::ConnectionRefusedError:
                   error_str = "连接被拒绝!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::RemoteHostClosedError:
                   error_str = " 远程主机已关闭!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SocketAccessError:
                   error_str = " 网络套接字访问错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SocketResourceError:
                   error_str = "网络套接字资源错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SocketTimeoutError:
                   error_str = "连接超时，请检查网络连接!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::DatagramTooLargeError:
                   error_str = "数据报包过大错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::NetworkError:
                   error_str = "网络错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::AddressInUseError:
                   error_str = "地址已占用错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SocketAddressNotAvailableError:
                   error_str = "地址不可用错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::UnsupportedSocketOperationError:
                   error_str = "不支持的套接字操作错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::UnfinishedSocketOperationError:
                   error_str = "未完成的套接字操作错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::ProxyAuthenticationRequiredError:
                   error_str = "代理认证要求错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SslHandshakeFailedError:
                   error_str = "SSL 会话失败错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::ProxyConnectionRefusedError:
                   error_str = "代理连接被拒绝错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::ProxyConnectionClosedError:
                   error_str = "代理连接已关闭错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::ProxyConnectionTimeoutError:
                   error_str = "代理连接超时错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::ProxyNotFoundError:
                   error_str = "代理未找到错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::ProxyProtocolError:
                   error_str = "代理协议错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::OperationError:
                   error_str = "操作错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SslInternalError:
                   error_str = "SSL 内部错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::SslInvalidUserDataError:
                   error_str = "SSL 无效用户数据错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::TemporaryError:
                   error_str = "临时错误!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               case QTcpSocket::HostNotFoundError:
                   error_str = "服务器未找到!";
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
               default:
                   QString tmp = "[其他错误]: ";
                   tmp += error_str;
                   qDebug() << error_str;
                   emit sig_con_success(false, error_str);
                   break;
           }
     });

    // 处理连接断开
    QObject::connect(&_socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected from server.";
    });
    //连接发送信号用来发送数据
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
}

TcpMgr::~TcpMgr()
{

}

void TcpMgr::registerMessageCallback(ReqId id, message_handle_t handle)
{
    _handlers[id] = handle;
}

void TcpMgr::unregisterMessageCallback(ReqId id)
{
    _handlers.remove(id);
}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter =  _handlers.find(id);
    if(find_iter == _handlers.end())
    {
         qDebug()<< "not found id ["<< id << "] to handle";
         return ;
    }

    find_iter.value()(len, data);
}

void TcpMgr::slot_tcp_connect(const QString &host, uint16_t port)
{
    _host = host;
    _port = port;

    _socket.connectToHost(host, port);
}

void TcpMgr::slot_send_data(ReqId reqId, QByteArray dataBytes)
{
    quint16 id = reqId;

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.length());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << id << len;

    block.append(dataBytes);

    _socket.write(block);

    qDebug() << "tcp mgr send byte data is " << block ;
}
