#ifndef PERSISTENTJSONSTATE_H
#define PERSISTENTJSONSTATE_H

#include "supportedsources.h"
#include <jsonstatefilepersistence.h>

class PersistentJsonState
{
public:
    PersistentJsonState(SupportedSourceTypes type, QString deviceName, QString deviceVersion);

    QJsonObject getJsonStructure() const;

    QJsonObject loadJsonState();
    void saveJsonState(QJsonObject state);
private:
    JsonStateFilePersistence m_jsonStatePersistenceHelper;
    QString m_stateFileName;
};

#endif // PERSISTENTJSONSTATE_H
