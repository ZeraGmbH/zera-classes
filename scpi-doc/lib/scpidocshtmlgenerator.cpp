#include "scpidocshtmlgenerator.h"
#include "sessionnamesmappingjson.h"
#include "modulemanagerconfig.h"
#include <QDir>
#include <qprocess.h>

void ScpiDocsHtmlGenerator::createScpiDocHtmls(QString zenuxRelease, QString docsPath, QString xmlDirPath)
{
    qputenv("QT_FATAL_CRITICALS", "1");

    SessionNamesMappingJson sessionNamesMapping(ModulemanagerConfig::getConfigFileNameFull());
    if(!sessionNamesMapping.storeMappedJsonFile(docsPath + "SessionNamesMapping.json"))
        qFatal("Session names mapping json file could not be created!");

    QString htmlDirPath = docsPath + "html-docs/";
    QDir().mkdir(htmlDirPath);

    QDir xmlDir(xmlDirPath);
    QString htmlPath;
    QString sessionName;
    for(auto xmlFile: xmlDir.entryInfoList({"*.xml"})) {
        htmlPath = htmlDirPath + xmlFile.fileName().replace("xml", "html");
        sessionName = sessionNamesMapping.getSessionNameForExternalUsers(xmlFile.fileName().replace("xml", "json"));
        convertXmlToHtml(zenuxRelease, xmlFile, sessionName, "false", htmlPath);
    }

    QFileInfo mtDefaultSessionXml(xmlDirPath + "/mt310s2-meas-session.xml");
    htmlPath = htmlDirPath + mtDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    convertXmlToHtml(zenuxRelease, mtDefaultSessionXml, "", "true", htmlPath);

    QFileInfo comDefaultSessionXml(xmlDirPath + "/com5003-meas-session.xml");
    htmlPath = htmlDirPath + comDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    convertXmlToHtml(zenuxRelease, comDefaultSessionXml, "", "true", htmlPath);

    xmlDir.removeRecursively();
}

void ScpiDocsHtmlGenerator::convertXmlToHtml(QString zenuxRelease, QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath)
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