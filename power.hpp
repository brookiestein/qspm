#ifndef POWER_HPP
#define POWER_HPP

#include <QDBusConnection>
#include <QMap>
#include <QObject>

#define DEST "org.freedesktop.login1"
#define PATH "/org/freedesktop/login1"
#define INTERFACE "org.freedesktop.login1.Manager"

class Power : public QObject
{
    Q_OBJECT
public:
    enum class Operation { SHUTDOWN = 0, REBOOT, SUSPEND, HIBERNATE };
    const static int DEFAULT_TIME = 60;

    explicit Power(QObject *parent = nullptr);

    QString operationToString(Operation operation);
    void performAction(Operation operation);

private:
    int m_timeToWait;
    Operation m_defaultOperation;
    QMap<Operation, QString> m_methods;
    QDBusConnection m_dbusConnection;

signals:
    void error(const QString &reason);
};

#endif // POWER_HPP
