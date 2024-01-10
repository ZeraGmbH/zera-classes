#include "scpiifaceexportgenerator.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QProcess>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption zenuxVersion("z", "Specify a Zenux release version after -z", "value");
    parser.addOption(zenuxVersion);
    parser.process(a);
    QString zenuxRelease = parser.value(zenuxVersion);

    qputenv("QT_FATAL_CRITICALS", "1");

    QString xmlDir = QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls/";
    QDir().mkdir(xmlDir);

    ScpiIfaceExportGenerator scpiIfaceExportGenerator;
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("mt310s2", xmlDir);
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("com5003", xmlDir);

    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls" << QStringLiteral(HTML_DOCS_PATH) << zenuxRelease);
    sh.waitForFinished();
    printf("%s", qPrintable(sh.readAll()));

    QDir dir(QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls");
    dir.removeRecursively();
}
