#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include <lxdmsessionchangeparam.h>
#include "testmodulemanager.h"
#include <QString>
#include <QHash>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> JsonByteArrayDumps;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(LxdmSessionChangeParam lxdmParam);
    void exportAll(QString xmlDir, QString snapshotDir);
    JsonByteArrayDumps getVeinDumps();
    JsonByteArrayDumps getDspMemDumps();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
    int getModuleConfigWriteCounts() const;
private:
    void prepareDirectory(QString path);
    LxdmSessionChangeParam m_lxdmParam;
    JsonByteArrayDumps m_veinDumps;
    JsonByteArrayDumps m_dspMemDumps;
    QList<TestModuleManager::TModuleInstances> m_instanceCounts;
    int m_moduleConfigFilesWritten = 0;
};

#endif // DEVICESEXPORTGENERATOR_H
