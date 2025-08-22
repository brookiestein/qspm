#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDBusMessage>
#include <QDBusError>
#include <QRegularExpression>

MainWindow::MainWindow(int timeToWait, Operation defaultOperation, QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_timeToWait(timeToWait)
    , m_defaultOperation(defaultOperation)
    , m_dbusConnection(QDBusConnection::systemBus())
{
    m_ui->setupUi(this);

    m_methods[Operation::SHUTDOWN] = "PowerOff";
    m_methods[Operation::REBOOT] = "Reboot";
    m_methods[Operation::SUSPEND] = "Suspend";
    m_methods[Operation::HIBERNATE] = "Hibernate";

    QString text;

    switch (defaultOperation)
    {
    case Operation::SHUTDOWN:
        text = tr("Shutting down in %1 seconds...");
        break;
    case Operation::REBOOT:
        text = tr("Rebooting in %1 seconds...");
        break;
    case Operation::SUSPEND:
        text = tr("Suspending in %1 seconds...");
        break;
    case Operation::HIBERNATE:
        text = tr("Hibernating in %1 seconds...");
        break;
    }

    text = text.arg(QString::number(timeToWait));
    m_ui->timerLabel->setText(text);
    m_timer.setInterval(1'000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    connect(m_ui->shutdownButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_ui->rebootButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_ui->suspendButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_ui->hibernateButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::performAction(Operation operation)
{
    QDBusMessage message = QDBusMessage::createMethodCall(DEST, PATH, INTERFACE, m_methods[operation]);
    message.setAutoStartService(false);
    message.setArguments({ "boolean:true" });
    m_dbusConnection.send(message);

    if (auto lastError = m_dbusConnection.lastError(); lastError.isValid()) {
        qCritical().noquote()
            << tr("An error occurred while asking the system to %1.\nDBus: %2")
                   .arg(operationToString(operation).toLower(),
                        lastError.message());
    }
}

QString MainWindow::operationToString(Operation operation)
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

void MainWindow::onTimeout()
{
    if (--m_timeToWait == 0) {
        m_timer.stop();
        performAction(m_defaultOperation);
    } else {
        QString text = m_ui->timerLabel->text();
        text.replace(QRegularExpression("\\d+"), QString::number(m_timeToWait));

        if (m_timeToWait == 1)
            text.replace(tr("seconds"), tr("second"));

        m_ui->timerLabel->setText(text);
    }
}

void MainWindow::onButtonClicked()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT_X(button, "This method should only be called as a slot of a QPushButton.", Q_FUNC_INFO);

    if (button == m_ui->shutdownButton) {
        performAction();
    } else if (button == m_ui->rebootButton) {
        performAction(Operation::REBOOT);
    } else if (button == m_ui->suspendButton) {
        performAction(Operation::SUSPEND);
    } else if (button == m_ui->hibernateButton) {
        performAction(Operation::HIBERNATE);
    } else {
        Q_ASSERT_X(false, "Unknown power-performer button.", Q_FUNC_INFO);
    }
}
