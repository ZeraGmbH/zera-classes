#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include <QString>
#include <QHash>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> VeinDumps;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(QString xmlDirPath);
    void exportAll(bool useDevModmanConfig);
    VeinDumps getVeinDumps();
private:
    QString m_xmlDirPath;
    VeinDumps m_veinDumps;
};

#endif // DEVICESEXPORTGENERATOR_H
