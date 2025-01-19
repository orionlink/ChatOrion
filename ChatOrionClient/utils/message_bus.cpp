#include "message_bus.h"

#include <QApplication>

MessageBus* MessageBus::m_instance = nullptr;

MessageBus::MessageBus(QObject* parent) : QObject(parent)
{
}

MessageBus* MessageBus::instance()
{
    if (!m_instance) {
        m_instance = new MessageBus(QCoreApplication::instance());
    }
    return m_instance;
}

void MessageBus::postMessage(const QString& command, const QVariant& data)
{
    if (m_handlers.contains(command)) {
        const auto& handlers = m_handlers[command];
        for (const auto& handler : handlers) {
            handler(data);
        }
    }
}
