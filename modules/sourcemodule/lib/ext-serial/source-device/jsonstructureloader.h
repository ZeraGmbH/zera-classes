#ifndef JSONSTRUCTURELOADER_H
#define JSONSTRUCTURELOADER_H

#include "supportedsources.h"
#include "sourceproperties.h"
#include <QString>
#include <QJsonObject>

class JsonStructureLoader
{
public:
    static QJsonObject loadJsonDefaultStructure(const SupportedSourceTypes &type);
    static QJsonObject loadJsonStructure(const SupportedSourceTypes &type, const QString &deviceName, const QString &deviceVersion);
    static QJsonObject loadJsonStructure(const SourceProperties &properties);
private:
    static QJsonObject loadJsonStructureFromFile(QString fileName);
};

#endif // JSONSTRUCTURELOADER_H
