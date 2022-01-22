#ifndef PERSISTENTJSONSTATE_H
#define PERSISTENTJSONSTATE_H

#include "jsonparamapi.h"
#include "source-device/supportedsources.h"
#include <jsonstatefilepersistence.h>

class PersistentJsonState
{
public:
    PersistentJsonState(SupportedSourceTypes type, QString deviceName, QString deviceVersion);

    QJsonObject getJsonStructure() const;

    JsonParamApi loadJsonState();
    void saveJsonState(JsonParamApi state);
private:
    JsonStateFilePersistence m_jsonStatePersistenceHelper;
    QString m_stateFileName;
};

#endif // PERSISTENTJSONSTATE_H
