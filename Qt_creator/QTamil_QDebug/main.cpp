#include <QCoreApplication>
#include <QTranslator>
#include <QDebug>

#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define RESET   "\033[0m"

#define QINFO(msg)    qDebug() << GREEN << QObject::tr("INFO: ") + msg << RESET
#define QWARN(msg)    qWarning() << YELLOW << QObject::tr("WARNING: ") + msg << RESET
#define QCRIT(msg)    qCritical() << RED << QObject::tr("CRITICAL: ") + msg << RESET

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Load Tamil translation
    QTranslator translator;
    if (translator.load("day1_ta_IN.qm")) { // make sure this file is in the same folder as ./day1
        a.installTranslator(&translator);
    }

    // Example messages
    qDebug() << QObject::tr("hellooo sab");
    QINFO(QObject::tr("Information message"));
    QWARN(QObject::tr("Warning message"));
    QCRIT(QObject::tr("Critical message"));

    return 0;
}
