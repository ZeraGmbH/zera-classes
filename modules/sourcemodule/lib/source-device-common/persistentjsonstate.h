#ifndef PERSISTENTJSONSTATE_H
#define PERSISTENTJSONSTATE_H

#include "jsonparamapi.h"
#include <jsonstatefilepersistence.h>

class PersistentJsonState
{
public:
    explicit PersistentJsonState(const QJsonObject deviceCapabilities);

    QJsonObject getJsonStructure() const;

    JsonParamApi loadJsonState();
    void saveJsonState(JsonParamApi state);
private:
    JsonStateFilePersistence m_jsonStatePersistenceHelper;
    QString m_stateFileName;
};

#endif // PERSISTENTJSONSTATE_H
