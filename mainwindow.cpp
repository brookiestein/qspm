#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDBusMessage>
#include <QDBusError>
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(int timeToWait, Power::Operation defaultOperation, Power &power, QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_timeToWait(timeToWait)
    , m_defaultOperation(defaultOperation)
    , m_power(power)
{
    m_ui->setupUi(this);

    QString text;

    switch (defaultOperation)
    {
    case Power::Operation::SHUTDOWN:
        text = tr("Shutting down in %1 seconds...");
        break;
    case Power::Operation::REBOOT:
        text = tr("Rebooting in %1 seconds...");
        break;
    case Power::Operation::SUSPEND:
        text = tr("Suspending in %1 seconds...");
        break;
    case Power::Operation::HIBERNATE:
        text = tr("Hibernating in %1 seconds...");
        break;
    }

    if (timeToWait == 1)
        text.replace(tr("seconds"), tr("second"));

    text = text.arg(QString::number(timeToWait));
    m_ui->timerLabel->setText(text);
    m_timer.setInterval(1'000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    connect(m_ui->shutdownButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_ui->rebootButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_ui->suspendButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_ui->hibernateButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);

    connect(&m_power, &Power::error, this, [this] (const QString &reason) {
        QMessageBox::critical(this, tr("Error"), reason);
    });
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setTimeToWait(int timeToWait)
{
    m_timeToWait = timeToWait;
}

void MainWindow::setDefaultOperation(Power::Operation operation)
{
    m_defaultOperation = operation;
}

void MainWindow::onTimeout()
{
    if (--m_timeToWait == 0) {
        m_timer.stop();
        m_power.performAction(m_defaultOperation);
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
        m_power.performAction(Power::Operation::SHUTDOWN);
    } else if (button == m_ui->rebootButton) {
        m_power.performAction(Power::Operation::REBOOT);
    } else if (button == m_ui->suspendButton) {
        m_power.performAction(Power::Operation::SUSPEND);
    } else if (button == m_ui->hibernateButton) {
        m_power.performAction(Power::Operation::HIBERNATE);
    } else {
        Q_ASSERT_X(false, "Unknown power-performer button.", Q_FUNC_INFO);
    }
}
