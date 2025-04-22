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
    DevicesExportGenerator(QString xmlDirPath);
    void exportAll(const LxdmSessionChangeParam &lxdmParam);
    VeinDumps getVeinDumps();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
private:
    QString m_xmlDirPath;
    VeinDumps m_veinDumps;
    QList<TestModuleManager::TModuleInstances> m_instanceCounts;
};

#endif // DEVICESEXPORTGENERATOR_H
