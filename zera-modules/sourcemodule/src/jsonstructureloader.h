#ifndef JSONSTRUCTURELOADER_H
#define JSONSTRUCTURELOADER_H

#include <QString>
#include <QJsonObject>
#include "supportedsources.h"

class JsonStructureLoader
{
public:
    static QJsonObject loadJsonDefaultStructure(SupportedSourceTypes type);
    static QJsonObject loadJsonStructure(SupportedSourceTypes type, QString deviceName);
private:
    static QJsonObject loadJsonStructureFromFile(QString fileName);
};

#endif // JSONSTRUCTURELOADER_H
