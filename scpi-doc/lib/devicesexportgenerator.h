#ifndef DEVICESEXPORTGENERATOR_H
#define DEVICESEXPORTGENERATOR_H

#include <QString>
#include <QFileInfo>

class DevicesExportGenerator
{
public:
    static void createDocs(QString zenuxRelease, QString htmlOutPath);
private:
    static void createHtml(QString zenuxRelease, QFileInfo sessionXml, QString sessionName, QString adjustment, QString htmlPath);
};

#endif // DEVICESEXPORTGENERATOR_H
