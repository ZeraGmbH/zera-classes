#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include <QString>
#include <QFileInfo>

typedef QHash<QString/*sessionName*/, QByteArray/*veinDump*/> VeinDumps;

class DevicesExportGenerator
{
public:
    DevicesExportGenerator(QString zenuxRelease, QString htmlOutPath);
    void exportAll();
    VeinDumps getVeinDumps();
private:
    void createHtml(QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath);
    QString m_zenuxRelease;
    QString m_htmlOutPath;
    VeinDumps m_veinDumps;
};

#endif // DEVICESEXPORTGENERATOR_H
