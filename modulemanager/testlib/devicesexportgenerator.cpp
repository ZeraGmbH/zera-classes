#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include <QProcess>
#include <QDir>

DevicesExportGenerator::DevicesExportGenerator(QString xmlDirPath, LxdmSessionChangeParam lxdmParam) :
    m_xmlDirPath(xmlDirPath),
    m_lxdmParam(lxdmParam)
{
}

void DevicesExportGenerator::exportAll()
{
    QDir().mkdir(m_xmlDirPath);

    SessionExportGenerator sessionExportGenerator(m_lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(m_xmlDirPath);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
        m_instanceCounts.append(sessionExportGenerator.getInstanceCountsOnModulesDestroyed());
    }
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}

QList<TestModuleManager::TModuleInstances> DevicesExportGenerator::getInstanceCountsOnModulesDestroyed()
{
    return m_instanceCounts;
}
