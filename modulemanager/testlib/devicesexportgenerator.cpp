#include "devicesexportgenerator.h"
#include "sessionexportgenerator.h"
#include <memoryalloctracker.h>
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
            MemoryAllocTracker memAllocTracker;
            memAllocTracker.start();
            sessionExportGenerator.changeSession(session);
            memAllocTracker.stop();
            AllocatedWithBacktraces backtraces = memAllocTracker.getAllocationsTimeSorted();

            m_memBacktraces.append(std::make_shared<BacktraceTreeGenerator>(backtraces));
            m_instanceCounts.append(sessionExportGenerator.getInstanceCountsOnModulesDestroyed());

            sessionExportGenerator.generateDevIfaceXml(m_xmlDirPath);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
        }
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

Backtraces DevicesExportGenerator::getMemoryLeakTrees()
{
    return m_memBacktraces;
}
