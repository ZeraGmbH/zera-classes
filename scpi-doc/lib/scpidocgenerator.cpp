#include "scpidocgenerator.h"
#include "scpiifaceexportgenerator.h"
#include "sessionnamesmappingjson.h"
#include <QProcess>
#include <QDir>

void ScpiDocGenerator::createDocs(QString zenuxRelease, QString htmlOutPath)
{
    qputenv("QT_FATAL_CRITICALS", "1");

    SessionNamesMappingJson sessionNamesMapping(ModulemanagerConfig::getConfigFileNameFull());
    if(!sessionNamesMapping.storeMappedJsonFile(htmlOutPath + "SessionNamesMapping.json"))
        qFatal("Session names mapping json file could not be created!");

    QString xmlDirPath = htmlOutPath + "scpi-xmls/";
    QDir().mkdir(xmlDirPath);
    QString htmlDirPath = htmlOutPath + "html-docs/";
    QDir().mkdir(htmlDirPath);

    ScpiIfaceExportGenerator scpiIfaceExportGenerator;
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("mt310s2", xmlDirPath);
    scpiIfaceExportGenerator.getAllSessionsScpiIfaceXmls("com5003", xmlDirPath);

    QDir xmlDir(xmlDirPath);
    QString htmlPath;
    QString sessionName;
    for(auto xmlFile: xmlDir.entryInfoList({"*.xml"})) {
        htmlPath = htmlDirPath + xmlFile.fileName().replace("xml", "html");
        sessionName = sessionNamesMapping.getSessionNameForExternalUsers(xmlFile.fileName().replace("xml", "json"));
        createHtml(zenuxRelease, xmlFile, sessionName, "false", htmlPath);
    }

    QFileInfo mtDefaultSessionXml(xmlDirPath + "/mt310s2-meas-session.xml");
    htmlPath = htmlDirPath + mtDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    createHtml(zenuxRelease, mtDefaultSessionXml, "", "true", htmlPath);

    QFileInfo comDefaultSessionXml(xmlDirPath + "/com5003-meas-session.xml");
    htmlPath = htmlDirPath + comDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    createHtml(zenuxRelease, comDefaultSessionXml, "", "true", htmlPath);

    xmlDir.removeRecursively();
}

void ScpiDocGenerator::createHtml(QString zenuxRelease, QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath)
{
    QProcess sh;
    QStringList paramList;
    paramList = QStringList()
                << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/create-html"
                << zenuxRelease
                << sessionXml.filePath()
                << sessionName
                << adjustment
                << htmlPath;
    sh.start("/bin/sh", paramList);
    sh.waitForFinished();
    qInfo("%s", qPrintable(sh.readAll()));
}
