#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include "sessionnamesmappingjson.h"
#include <QProcess>
#include <QDir>

DevicesExportGenerator::DevicesExportGenerator(QString zenuxRelease, QString htmlOutPath) :
    m_zenuxRelease(zenuxRelease),
    m_htmlOutPath(htmlOutPath)
{
}

void DevicesExportGenerator::exportAll()
{
    qputenv("QT_FATAL_CRITICALS", "1");

    SessionNamesMappingJson sessionNamesMapping(ModulemanagerConfig::getConfigFileNameFull());
    if(!sessionNamesMapping.storeMappedJsonFile(m_htmlOutPath + "SessionNamesMapping.json"))
        qFatal("Session names mapping json file could not be created!");

    QString xmlDirPath = m_htmlOutPath + "scpi-xmls/";
    QDir().mkdir(xmlDirPath);
    QString htmlDirPath = m_htmlOutPath + "html-docs/";
    QDir().mkdir(htmlDirPath);

    SessionExportGenerator sessionExportGenerator;
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(xmlDirPath);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
    }

    QDir xmlDir(xmlDirPath);
    QString htmlPath;
    QString sessionName;
    for(auto xmlFile: xmlDir.entryInfoList({"*.xml"})) {
        htmlPath = htmlDirPath + xmlFile.fileName().replace("xml", "html");
        sessionName = sessionNamesMapping.getSessionNameForExternalUsers(xmlFile.fileName().replace("xml", "json"));
        createHtml(xmlFile, sessionName, "false", htmlPath);
    }

    QFileInfo mtDefaultSessionXml(xmlDirPath + "/mt310s2-meas-session.xml");
    htmlPath = htmlDirPath + mtDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    createHtml(mtDefaultSessionXml, "", "true", htmlPath);

    QFileInfo comDefaultSessionXml(xmlDirPath + "/com5003-meas-session.xml");
    htmlPath = htmlDirPath + comDefaultSessionXml.fileName().replace("meas-session.xml", "adjustment.html");
    createHtml(comDefaultSessionXml, "", "true", htmlPath);

    xmlDir.removeRecursively();
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}

void DevicesExportGenerator::createHtml(QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath)
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
