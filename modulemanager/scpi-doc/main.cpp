#include "factoryserviceinterfacessingleton.h"
#include "factoryserviceinterfaces.h"
#include "scpiifaceexportgenerator.h"
#include "sessionnamesmappingjson.h"
#include "zera-jsonfileloader.h"
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

    QString xmlDir = QStringLiteral(HTML_DOCS_PATH) + "scpi-xmls/";
    QDir().mkdir(xmlDir);

    FactoryServiceInterfacesSingleton::setInstance(std::make_unique<FactoryServiceInterfaces>());
    ScpiIfaceExportGenerator scpiIfaceExportGenerator;
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("mt310s2", xmlDir);
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("com5003", xmlDir);

    QJsonObject mappedJson = SessionNamesMappingJson::createSessionNamesMappingJsonAllDevices(ModulemanagerConfig::getConfigFileNameFull());
    if(!cJsonFileLoader::storeJsonFile(QStringLiteral(HTML_DOCS_PATH) + "SessionNamesMapping.json", mappedJson))
        qFatal("Session names mapping json file could not be created!");

    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls" << QStringLiteral(HTML_DOCS_PATH) << zenuxRelease);
    sh.waitForFinished();
    printf("%s", qPrintable(sh.readAll()));

    QDir dir(QStringLiteral(HTML_DOCS_PATH) + "scpi-xmls");
    dir.removeRecursively();
}
