#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include <QProcess>
#include <QDir>

DevicesExportGenerator::DevicesExportGenerator(LxdmSessionChangeParam lxdmParam) :
    m_lxdmParam(lxdmParam)
{
}

void DevicesExportGenerator::exportDevIfaceXmls(QString xmlDir)
{
    m_xmlDir = xmlDir;
    QDir().mkdir(m_xmlDir);
    exportSelected(true, false);
}

void DevicesExportGenerator::exportAll(QString xmlDir)
{
    m_xmlDir = xmlDir;
    QDir().mkdir(m_xmlDir);
    exportSelected(true, true);
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}

QList<TestModuleManager::TModuleInstances> DevicesExportGenerator::getInstanceCountsOnModulesDestroyed()
{
    return m_instanceCounts;
}

void DevicesExportGenerator::exportSelected(bool exportXmls, bool exportVeindumps)
{
    SessionExportGenerator sessionExportGenerator(m_lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            if(exportXmls)
                sessionExportGenerator.generateDevIfaceXml(m_xmlDir);
            if(exportVeindumps)
                m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
        m_instanceCounts.append(sessionExportGenerator.getInstanceCountsOnModulesDestroyed());
    }
}
