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
    exportEnabler selectionFlags {true, false};
    exportSelected(selectionFlags);
}

void DevicesExportGenerator::exportAll(QString xmlDir)
{
    m_xmlDir = xmlDir;
    QDir().mkdir(m_xmlDir);
    exportEnabler selectionFlags {true, true};
    exportSelected(selectionFlags);
}

VeinDumps DevicesExportGenerator::getVeinDumps()
{
    return m_veinDumps;
}

QList<TestModuleManager::TModuleInstances> DevicesExportGenerator::getInstanceCountsOnModulesDestroyed()
{
    return m_instanceCounts;
}

void DevicesExportGenerator::exportSelected(exportEnabler selectionFlags)
{
    SessionExportGenerator sessionExportGenerator(m_lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            if(selectionFlags.exportXmls)
                sessionExportGenerator.generateDevIfaceXml(m_xmlDir);
            if(selectionFlags.exportVeindumps)
                m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
        m_instanceCounts.append(sessionExportGenerator.getInstanceCountsOnModulesDestroyed());
    }
}
