#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include "abstractfactoryserviceinterfaces.h"
#include "factoryserviceinterfaces.h"
#include <lxdmsessionchangeparam.h>
#include <QString>
#include <QHash>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> VeinDumps;
typedef QHash<QString/*sessionName*/, QString/*scpiResponse*/> ScpiIoDumps;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(QString xmlDirPath);
    void exportAll(const LxdmSessionChangeParam &lxdmParam,
                   AbstractFactoryServiceInterfacesPtr serviceInterFaceFactory = std::make_shared<FactoryServiceInterfaces>(),
                   const QByteArray scpiCmd = "");
    VeinDumps getVeinDumps();
    ScpiIoDumps getScpiIoDumps();
private:
    QString m_xmlDirPath;
    VeinDumps m_veinDumps;
    ScpiIoDumps m_scpiIoDumps;
};

#endif // DEVICESEXPORTGENERATOR_H
