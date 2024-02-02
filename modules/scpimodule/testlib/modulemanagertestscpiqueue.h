#ifndef MODULEMANAGERTESTSCPIQUEUE_H
#define MODULEMANAGERTESTSCPIQUEUE_H

#include "basemodule.h"
#include <ve_eventhandler.h>
#include <vs_veinhash.h>
#include <QObject>
#include <QSet>

class ModuleManagerTestScpiQueue : public QObject
{
    Q_OBJECT
public:
    ModuleManagerTestScpiQueue();
    void addModule(cBaseModule* module, QString configFileFullPath);
    VeinStorage::VeinHash* getStorageSystem();
private:
    void addSystem(VeinEvent::EventSystem* subsystem);

    VeinEvent::EventHandler m_eventHandler;
    VeinStorage::VeinHash m_storageSystem;
    QSet<VeinEvent::EventSystem*> m_addedSubsystems;
};

#endif // MODULEMANAGERTESTSCPIQUEUE_H
