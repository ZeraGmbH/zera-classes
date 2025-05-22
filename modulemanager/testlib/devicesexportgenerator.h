#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include "testmodulemanager.h"
#include <backtracetreegenerator.h>
#include <lxdmsessionchangeparam.h>
#include <QString>
#include <QHash>
#include <memory>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> VeinDumps;
typedef QList<std::shared_ptr<BacktraceTreeGenerator>> Backtraces;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(QString xmlDirPath);
    void exportAll(const LxdmSessionChangeParam &lxdmParam);
    VeinDumps getVeinDumps();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
    Backtraces getMemoryLeakTrees();
private:
    QString m_xmlDirPath;
    VeinDumps m_veinDumps;
    Backtraces m_memBacktraces;
    QList<TestModuleManager::TModuleInstances> m_instanceCounts;
};

#endif // DEVICESEXPORTGENERATOR_H
