#ifndef SOURCEPERSISTENTJSONSTATE_H
#define SOURCEPERSISTENTJSONSTATE_H

#include "supportedsources.h"
#include <jsonstatefilepersistence.h>

class SourcePersistentJsonState
{
public:
    SourcePersistentJsonState(SupportedSourceTypes type);

    QJsonObject getJsonStructure();

    QJsonObject loadJsonState();
    void saveJsonState(QJsonObject state);
private:
    JsonStateFilePersistence m_jsonStatePersistenceHelper;
    QString m_stateFileName;
};

#endif // SOURCEPERSISTENTJSONSTATE_H
