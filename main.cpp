#include "config.hpp"
#include "mainwindow.hpp"
#include "power.hpp"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QLocale>
#include <QTranslator>

void setParserUp(QCommandLineParser &parser);

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
    setParserUp(parser);
    parser.process(a);

    int timeToWait = Power::DEFAULT_TIME;
    auto defaultOperation = Power::Operation::SHUTDOWN;

    if (QString time = parser.value("time-to-wait"); !time.isEmpty()) {
        bool ok {};
        timeToWait = time.toInt(&ok);
        if (not ok) {
            qCritical().noquote() << QObject::tr("Time to wait: '%1' is not valid.").arg(time);
            return -1;
        }
    }

    Power power;

    if (parser.isSet("poweroff")) {
        power.performAction(Power::Operation::SHUTDOWN);
        return 0;
    } else if (parser.isSet("reboot")) {
        power.performAction(Power::Operation::REBOOT);
        return 0;
    } else if (parser.isSet("suspend")) {
        power.performAction(Power::Operation::SUSPEND);
        return 0;
    } else if (parser.isSet("hibernate")) {
        power.performAction(Power::Operation::HIBERNATE);
        return 0;
    }

    if (QString op = parser.value("default-operation").toLower(); !op.isEmpty() && op != "shutdown") {
        if (op == "reboot")
            defaultOperation = Power::Operation::REBOOT;
        else if (op == "suspend")
            defaultOperation = Power::Operation::SUSPEND;
        else if (op == "hibernate")
            defaultOperation = Power::Operation::HIBERNATE;
    }

    MainWindow w(timeToWait, defaultOperation, power);
    w.show();
    return a.exec();
}

void setParserUp(QCommandLineParser &parser)
{
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
        ),
        QCommandLineOption(
            QStringList() << "p" << "poweroff",
            QObject::tr("Ask the system to power off."),
            "poweroff"
        ),
        QCommandLineOption(
            QStringList() << "r" << "reboot",
            QObject::tr("Ask the system to reboot."),
            "reboot"
        ),
        QCommandLineOption(
            QStringList() << "s" << "suspend",
            QObject::tr("Ask the system to suspend."),
            "suspend"
        ),
        QCommandLineOption(
            QStringList() << "H" << "hibernate",
            QObject::tr("Ask the system to hibernate."),
            "hibernate"
        ),
    });
    parser.addVersionOption();
}
