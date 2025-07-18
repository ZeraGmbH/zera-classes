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
    void exportAll(QString xmlDir, QString snapshotDir);
    VeinDumps getVeinDumps();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
private:
    struct exportEnabler {
        bool exportXmls;
        bool exportVeindumps;
        bool exportSnapshots;
    };

    void exportSelected(exportEnabler selectionFlags);

    LxdmSessionChangeParam m_lxdmParam;
    QString m_xmlDir;
    QString m_snapshotDir;
    VeinDumps m_veinDumps;
    QList<TestModuleManager::TModuleInstances> m_instanceCounts;
};

#endif // DEVICESEXPORTGENERATOR_H
