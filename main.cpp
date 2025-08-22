#include "mainwindow.hpp"
#include "config.hpp"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName(PROJECT_NAME);
    QApplication::setApplicationVersion(PROJECT_VERSION);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "spm_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOptions({
        QCommandLineOption(
            QStringList() << "d" << "default-operation",
            QObject::tr("Default operation to be performed when timer ends. "
                        "Available: shutdown (default), reboot, suspend, and hibernate."),
            "default-operation"
        ),
        QCommandLineOption(
            QStringList() << "t" << "time-to-wait",
            QObject::tr("Time to wait before doing the default operation."),
            "time-to-wait"
        )
    });
    parser.addVersionOption();
    parser.process(a);

    int timeToWait = MainWindow::DEFAULT_TIME;
    auto defaultOperation = MainWindow::Operation::SHUTDOWN;

    if (QString time = parser.value("time-to-wait"); !time.isEmpty()) {
        bool ok {};
        timeToWait = time.toInt(&ok);
        if (not ok) {
            qCritical().noquote() << QObject::tr("Time to wait: '%1' is not valid.").arg(time);
            return -1;
        }
    }

    if (QString op = parser.value("default-operation").toLower(); !op.isEmpty() && op != "shutdown") {
        if (op == "reboot")
            defaultOperation = MainWindow::Operation::REBOOT;
        else if (op == "suspend")
            defaultOperation = MainWindow::Operation::SUSPEND;
        else if (op == "hibernate")
            defaultOperation = MainWindow::Operation::HIBERNATE;
    }

    MainWindow w(timeToWait, defaultOperation);
    w.show();
    return a.exec();
}
