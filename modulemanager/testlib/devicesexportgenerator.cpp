#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include <QProcess>
#include <QDir>

DevicesExportGenerator::DevicesExportGenerator(QString xmlDirPath) :
    m_xmlDirPath(xmlDirPath)
{
}

void DevicesExportGenerator::exportAll(const LxdmSessionChangeParam &lxdmParam)
{
    QDir().mkdir(m_xmlDirPath);

    SessionExportGenerator sessionExportGenerator(lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(m_xmlDirPath);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
    }
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}
