#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include <QProcess>
#include <QDir>

DevicesExportGenerator::DevicesExportGenerator(LxdmSessionChangeParam lxdmParam) :
    m_lxdmParam(lxdmParam)
{
}

void DevicesExportGenerator::exportAll(QString xmlDir, QString snapshotDir)
{
    m_xmlDir = xmlDir;
    QDir().mkdir(m_xmlDir);
    m_snapshotDir = snapshotDir;
    QDir().mkdir(m_snapshotDir);

    SessionExportGenerator sessionExportGenerator(m_lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(m_xmlDir);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
            sessionExportGenerator.generateSnapshotJsons(m_snapshotDir);
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
