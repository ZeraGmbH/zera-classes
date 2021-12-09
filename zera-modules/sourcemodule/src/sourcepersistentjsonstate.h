#ifndef SOURCEPERSISTENTJSONSTATE_H
#define SOURCEPERSISTENTJSONSTATE_H

#include "supportedsources.h"
#include <QJsonObject>

class SourcePersistentJsonState
{
public:
    SourcePersistentJsonState(SupportedSourceTypes type);

    QJsonObject getJsonStructure();

    QJsonObject loadJsonState();
    void saveJsonState(QJsonObject state);
private:
    SupportedSourceTypes m_sourceType;
    QJsonObject m_jsonParamStructure;
};

#endif // SOURCEPERSISTENTJSONSTATE_H
