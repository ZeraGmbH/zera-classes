#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include <lxdmsessionchangeparam.h>
#include <QString>
#include <QHash>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> VeinDumps;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(QString xmlDirPath);
    void exportAll(bool useDevModmanConfig, const LxdmSessionChangeParam &lxdmParam);
    VeinDumps getVeinDumps();
private:
    QString m_xmlDirPath;
    VeinDumps m_veinDumps;
};

#endif // DEVICESEXPORTGENERATOR_H
