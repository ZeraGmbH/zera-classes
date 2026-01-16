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
    prepareDirectory(xmlDir);
    prepareDirectory(snapshotDir);

    SessionExportGenerator sessionExportGenerator(m_lxdmParam);
    QStringList devices = {"mt310s2", "com5003"};
    for(const QString &device: devices) {
        sessionExportGenerator.setDevice(device);
        for(const QString &session: sessionExportGenerator.getAvailableSessions()) {
            sessionExportGenerator.changeSession(session);
            sessionExportGenerator.generateDevIfaceXml(xmlDir);
            m_veinDumps[session] = sessionExportGenerator.getVeinDump();
            sessionExportGenerator.generateSnapshotJsons(snapshotDir);
        }
        m_instanceCounts.append(sessionExportGenerator.getInstanceCountsOnModulesDestroyed());
        m_moduleConfigFilesWritten += sessionExportGenerator.getModuleConfigWriteCounts();
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

int DevicesExportGenerator::getModuleConfigWriteCounts() const
{
    return m_moduleConfigFilesWritten;
}

void DevicesExportGenerator::prepareDirectory(QString path)
{
    QDir dir(path);
    if(dir.exists())
        dir.removeRecursively(); //Removes the directory, including all its contents
    dir.mkpath(path);
}
