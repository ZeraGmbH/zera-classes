#include "scpiifaceexportgenerator.h"
#include "sessionnamesmappingjson.h"
#include "zera-jsonfileloader.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QProcess>


void createHtml(QString zenuxRelease, QFileInfo sessionXml, QString adjustment, QString htmlPath)
{
    QProcess sh;
    QStringList paramList;
    paramList = QStringList()
                << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/create-html"
                << zenuxRelease
                << sessionXml.filePath()
                << adjustment
                << htmlPath;
    sh.start("/bin/sh", paramList);
    sh.waitForFinished();
    printf("%s", qPrintable(sh.readAll()));
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption zenuxVersion("z", "Specify a Zenux release version after -z", "value");
    parser.addOption(zenuxVersion);
    parser.process(a);
    QString zenuxRelease = parser.value(zenuxVersion);

    qputenv("QT_FATAL_CRITICALS", "1");

    QJsonObject mappedJson = SessionNamesMappingJson::createSessionNamesMappingJsonAllDevices(ModulemanagerConfig::getConfigFileNameFull());
    if(!cJsonFileLoader::storeJsonFile(QStringLiteral(HTML_DOCS_PATH) + "SessionNamesMapping.json", mappedJson))
        qFatal("Session names mapping json file could not be created!");

    QString xmlDirPath = QStringLiteral(HTML_DOCS_PATH) + "scpi-xmls/";
    QDir().mkdir(xmlDirPath);
    QString htmlDirPath = QStringLiteral(HTML_DOCS_PATH) + "html-docs/";
    QDir().mkdir(htmlDirPath);

    ScpiIfaceExportGenerator scpiIfaceExportGenerator;
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("mt310s2", xmlDirPath);
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("com5003", xmlDirPath);

    QDir xmlDir(xmlDirPath);
    QString htmlPath;
    for(auto xmlFile: xmlDir.entryInfoList({"*.xml"})) {
        htmlPath = htmlDirPath + xmlFile.fileName().replace("xml", "html");
        createHtml(zenuxRelease, xmlFile, "false", htmlPath);
    }

    QFileInfo mtDefaultSessionXml(xmlDirPath + "/mt310s2-meas-session.xml");
    htmlPath = htmlDirPath + mtDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    createHtml(zenuxRelease, mtDefaultSessionXml, "true", htmlPath);

    QFileInfo comDefaultSessionXml(xmlDirPath + "/com5003-meas-session.xml");
    htmlPath = htmlDirPath + comDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    createHtml(zenuxRelease, comDefaultSessionXml, "true", htmlPath);

    xmlDir.removeRecursively();
}
