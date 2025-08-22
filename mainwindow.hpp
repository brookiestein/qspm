#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QDBusConnection>
#include <QMainWindow>
#include <QTimer>

#include "power.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int timeToWait, Power::Operation operation, Power &power, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    int m_timeToWait;
    Power::Operation m_defaultOperation;
    QTimer m_timer;
    Power &m_power;

public:
    void setTimeToWait(int timeToWait);
    void setDefaultOperation(Power::Operation operation);

private slots:
    void onTimeout();
    void onButtonClicked();
};
#endif // MAINWINDOW_HPP
