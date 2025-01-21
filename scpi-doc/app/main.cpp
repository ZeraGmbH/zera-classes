#include "devicesexportgenerator.h"
#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption zenuxVersion("z", "Specify a Zenux release version after -z", "value");
    parser.addOption(zenuxVersion);
    parser.process(a);
    QString zenuxRelease = parser.value(zenuxVersion);
    DevicesExportGenerator devicesExportGenerator(zenuxRelease, QStringLiteral(HTML_DOCS_PATH));
    devicesExportGenerator.exportAll();
}
