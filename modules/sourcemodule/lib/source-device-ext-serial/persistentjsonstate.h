#ifndef PERSISTENTJSONSTATE_H
#define PERSISTENTJSONSTATE_H

#include "jsonparamapi.h"
#include "supportedsources.h"
#include "sourceproperties.h"
#include <jsonstatefilepersistence.h>

class PersistentJsonState
{
public:
    PersistentJsonState(const QJsonObject deviceCapabilities);
    PersistentJsonState(SupportedSourceTypes type, QString deviceName, QString deviceVersion);
    PersistentJsonState(SourceProperties properties);

    QJsonObject getJsonStructure() const;

    JsonParamApi loadJsonState();
    void saveJsonState(JsonParamApi state);
private:
    void init(SupportedSourceTypes type, QString deviceName, QString deviceVersion);

    JsonStateFilePersistence m_jsonStatePersistenceHelper;
    QString m_stateFileName;
};

#endif // PERSISTENTJSONSTATE_H
