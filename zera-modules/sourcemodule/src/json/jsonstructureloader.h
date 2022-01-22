#ifndef JSONSTRUCTURELOADER_H
#define JSONSTRUCTURELOADER_H

#include "source-device/supportedsources.h"
#include <QString>
#include <QJsonObject>

class JsonStructureLoader
{
public:
    static QJsonObject loadJsonDefaultStructure(SupportedSourceTypes type);
    static QJsonObject loadJsonStructure(SupportedSourceTypes type, QString deviceName, QString deviceVersion);
private:
    static QJsonObject loadJsonStructureFromFile(QString fileName);
};

#endif // JSONSTRUCTURELOADER_H
