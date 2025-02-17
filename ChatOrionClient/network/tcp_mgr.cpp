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

//        QDataStream stream(&_buffer, QIODevice::ReadOnly);
//        stream.setVersion(QDataStream::Qt_5_14);
//        stream.setByteOrder(QDataStream::BigEndian);

//        forever
//        {
//            // 先解析头部
//            if (!_b_recv_pending)
//            {
//                 // 检查缓冲区中的数据是否足够解析出一个消息头（消息ID + 消息长度）
//                if (_buffer.size() < static_cast<int>(sizeof(qint16) * 2))
//                {
//                    return;
//                }

//                stream >> _message_id >> _message_len;

//                //将buffer 中的前四个字节移除
//                _buffer = _buffer.mid(sizeof(quint16) * 2);

//                // 输出读取的数据
//                qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
//            }

//            if (_buffer.size() < _message_len)
//            {
//                _b_recv_pending = true;
//                return;
//            }

//            _b_recv_pending = false;

//            // 读取消息体
//            QByteArray messageBody = _buffer.mid(0, _message_len);
//            qDebug() << "receive body msg is " << messageBody ;

//            _buffer = _buffer.mid(_message_len);

//            handleMsg(ReqId(_message_id),_message_len, messageBody);
//        }

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

    //5.15 之后版本
//       QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
//           Q_UNUSED(socketError)
//           qDebug() << "Error:" << _socket.errorString();
//       });

       // 处理错误（适用于Qt 5.15之前的版本）
    QObject::connect(&_socket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
                        [&](QTcpSocket::SocketError socketError) {
           qDebug() << "Error:" << _socket.errorString() ;
           switch (socketError) {
               case QTcpSocket::ConnectionRefusedError:
                   qDebug() << "Connection Refused!";
                   emit sig_con_success(false);
                   break;
               case QTcpSocket::RemoteHostClosedError:
                   qDebug() << "Remote Host Closed Connection!";
                   break;
               case QTcpSocket::HostNotFoundError:
                   qDebug() << "Host Not Found!";
                   emit sig_con_success(false);
                   break;
               case QTcpSocket::SocketTimeoutError:
                   qDebug() << "Connection Timeout!";
                   emit sig_con_success(false);
                   break;
               case QTcpSocket::NetworkError:
                   qDebug() << "Network Error!";
                   break;
               default:
                   qDebug() << "Other Error!";
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
