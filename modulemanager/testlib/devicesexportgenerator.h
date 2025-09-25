#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include <lxdmsessionchangeparam.h>
#include "testmodulemanager.h"
#include <QString>
#include <QHash>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> VeinDumps;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(LxdmSessionChangeParam lxdmParam);
    void exportAll(QString xmlDir, QString snapshotDir);
    VeinDumps getVeinDumps();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
private:
    void prepareDirectory(QString path);
    LxdmSessionChangeParam m_lxdmParam;
    VeinDumps m_veinDumps;
    QList<TestModuleManager::TModuleInstances> m_instanceCounts;
};

#endif // DEVICESEXPORTGENERATOR_H
