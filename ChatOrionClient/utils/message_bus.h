// MessageBus.h
#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QPointer>
#include <functional>

#include "message_commands.h"

class MessageBus : public QObject
{
    Q_OBJECT
public:
    static MessageBus* instance();

    // 注册消息处理函数
    template<typename Class, typename Func>
    void registerHandler(const QString& command, Class* receiver, Func&& handler) {
        QPointer<QObject> weakReceiver(receiver);

        if constexpr (std::is_member_function_pointer_v<Func>) {
            auto wrapper = [weakReceiver, handler](const QVariant& data) {
                if (auto obj = weakReceiver.data()) {
                    // 将 QObject* 转换回正确的类型
                    if (auto typedObj = qobject_cast<Class*>(obj)) {
                        (typedObj->*handler)(data);
                    }
                }
            };
            m_handlers[command].append(wrapper);
        } else {
            auto wrapper = [weakReceiver, handler = std::forward<Func>(handler)](const QVariant& data) {
                if (!weakReceiver.isNull()) {
                    handler(data);
                }
            };
            m_handlers[command].append(wrapper);
        }
    }
//    template<typename Func>
//    void registerHandler(const QString& command, QObject* receiver, Func handler) {
//        // 使用 QPointer 来追踪对象
//        QPointer<QObject> weakReceiver(receiver);
//        auto wrapper = [weakReceiver, handler](const QVariant& data) {
//            // 检查对象是否还存在
//            if (!weakReceiver.isNull()) {
//                handler(data);
//            }
//        };
//        m_handlers[command].append(wrapper);
//    }

    // 发送消息的静态函数
    template<typename T>
    static void sendMessage(const QString& command, const T& data) {
        instance()->postMessage(command, QVariant::fromValue(data));
    }

private:
    explicit MessageBus(QObject* parent = nullptr);
    void postMessage(const QString& command, const QVariant& data);

    static MessageBus* m_instance;
    QMap<QString, QList<std::function<void(const QVariant&)>>> m_handlers;
};

#endif // MESSAGEBUS_H
