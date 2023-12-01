#ifndef MODULEMANAGERFORTEST_H
#define MODULEMANAGERFORTEST_H

#include "basemodule.h"
#include <ve_eventhandler.h>
#include <vs_veinhash.h>
#include <QObject>
#include <QSet>

class ModuleManagerForTest : public QObject
{
    Q_OBJECT
public:
    ModuleManagerForTest();
    void addModule(cBaseModule* module, QString configFileFullPath);
    VeinStorage::VeinHash* getStorageSystem();
private:
    void addSystem(VeinEvent::EventSystem* subsystem);

    VeinEvent::EventHandler m_eventHandler;
    VeinStorage::VeinHash m_storageSystem;
    QSet<VeinEvent::EventSystem*> m_addedSubsystems;
};

#endif // MODULEMANAGERFORTEST_H
