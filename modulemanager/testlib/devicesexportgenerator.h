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
    void exportDevIfaceXmls(QString xmlDir);
    void exportAll(QString xmlDir);
    VeinDumps getVeinDumps();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
private:
    void exportSelected(bool exportXmls, bool exportVeindumps);
    LxdmSessionChangeParam m_lxdmParam;
    QString m_xmlDir;
    VeinDumps m_veinDumps;
    QList<TestModuleManager::TModuleInstances> m_instanceCounts;
};

#endif // DEVICESEXPORTGENERATOR_H
