#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QDBusConnection>
#include <QMainWindow>
#include <QTimer>

#define DEST "org.freedesktop.login1"
#define PATH "/org/freedesktop/login1"
#define INTERFACE "org.freedesktop.login1.Manager"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class Operation { SHUTDOWN = 0, REBOOT, SUSPEND, HIBERNATE };
    const static int DEFAULT_TIME = 60;
    MainWindow(int timeToWait = DEFAULT_TIME, Operation defaultOperation = Operation::SHUTDOWN, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    int m_timeToWait;
    Operation m_defaultOperation;
    QTimer m_timer;
    QDBusConnection m_dbusConnection;
    QMap<Operation, QString> m_methods;

public:
    void performAction(Operation operation = Operation::SHUTDOWN);
    QString operationToString(Operation operation);

private slots:
    void onTimeout();
    void onButtonClicked();
};
#endif // MAINWINDOW_HPP
