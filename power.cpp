#include "power.hpp"

#include <QDBusMessage>

Power::Power(QObject *parent)
    : QObject{parent}
    , m_dbusConnection {QDBusConnection::systemBus()}
{
    m_methods[Operation::SHUTDOWN] = "PowerOff";
    m_methods[Operation::REBOOT] = "Reboot";
    m_methods[Operation::SUSPEND] = "Suspend";
    m_methods[Operation::HIBERNATE] = "Hibernate";
}

QString Power::operationToString(Operation operation)
{
    switch (operation)
    {
    case Operation::SHUTDOWN:
        return "Shutdown";
    case Operation::REBOOT:
        return "Reboot";
    case Operation::SUSPEND:
        return "Suspend";
    case Operation::HIBERNATE:
        return "Hibernate";
    }

    return QString();
}

void Power::performAction(Operation operation)
{
    QDBusMessage message = QDBusMessage::createMethodCall(DEST, PATH, INTERFACE, m_methods[operation]);
    message.setAutoStartService(false);
    message.setArguments({ "boolean:true" });
    m_dbusConnection.send(message);

    if (auto lastError = m_dbusConnection.lastError(); lastError.isValid()) {
        emit error(
            tr("An error occurred while asking the system to %1.\nDBus: %2")
                .arg(operationToString(operation).toLower(),
                     lastError.message())
        );
    }
}
