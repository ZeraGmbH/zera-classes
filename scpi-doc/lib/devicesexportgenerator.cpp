#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include "sessionnamesmappingjson.h"
#include <QProcess>
#include <QDir>

DevicesExportGenerator::DevicesExportGenerator(QString zenuxRelease, QString htmlOutPath) :
    m_zenuxRelease(zenuxRelease),
    m_htmlOutPath(htmlOutPath)
{
    qputenv("QT_FATAL_CRITICALS", "1");
    m_xmlDirPath = m_htmlOutPath + "scpi-xmls/";
}

void DevicesExportGenerator::exportAll()
{
    QDir().mkdir(m_xmlDirPath);

    SessionExportGenerator sessionExportGenerator;
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(m_xmlDirPath);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
    }
    createScpiDocHtmls();
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}

void DevicesExportGenerator::createScpiDocHtmls()
{
    SessionNamesMappingJson sessionNamesMapping(ModulemanagerConfig::getConfigFileNameFull());
    if(!sessionNamesMapping.storeMappedJsonFile(m_htmlOutPath + "SessionNamesMapping.json"))
        qFatal("Session names mapping json file could not be created!");

    QString htmlDirPath = m_htmlOutPath + "html-docs/";
    QDir().mkdir(htmlDirPath);

    QDir xmlDir(m_xmlDirPath);
    QString htmlPath;
    QString sessionName;
    for(auto xmlFile: xmlDir.entryInfoList({"*.xml"})) {
        htmlPath = htmlDirPath + xmlFile.fileName().replace("xml", "html");
        sessionName = sessionNamesMapping.getSessionNameForExternalUsers(xmlFile.fileName().replace("xml", "json"));
        convertXmlToHtml(xmlFile, sessionName, "false", htmlPath);
    }

    QFileInfo mtDefaultSessionXml(m_xmlDirPath + "/mt310s2-meas-session.xml");
    htmlPath = htmlDirPath + mtDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    convertXmlToHtml(mtDefaultSessionXml, "", "true", htmlPath);

    QFileInfo comDefaultSessionXml(m_xmlDirPath + "/com5003-meas-session.xml");
    htmlPath = htmlDirPath + comDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    convertXmlToHtml(comDefaultSessionXml, "", "true", htmlPath);

    xmlDir.removeRecursively();
}

void DevicesExportGenerator::convertXmlToHtml(QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath)
{
    QProcess sh;
    QStringList paramList;
    paramList = QStringList()
                << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/create-html"
                << m_zenuxRelease
                << sessionXml.filePath()
                << sessionName
                << adjustment
                << htmlPath;
    sh.start("/bin/sh", paramList);
    sh.waitForFinished();
    qInfo("%s", qPrintable(sh.readAll()));
}
