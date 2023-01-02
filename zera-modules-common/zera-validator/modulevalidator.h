#ifndef MODULEVALIDATOR_H
#define MODULEVALIDATOR_H

#include <vfcommandfiltereventsystem.h>
#include <vfmoduleparameter.h>
#include <ve_storagesystem.h>
#include <QHash>

class ModuleValidator : public VeinCommandFilterEventSystem
{
    Q_OBJECT
public:
    ModuleValidator(int entityId, VeinEvent::StorageSystem* storageSystem);
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
    void setParameterHash(QHash<QString, cVeinModuleParameter *>&parameterhash);
private:
    int m_entityId;
    VeinEvent::StorageSystem* m_storageSystem;
    QHash<QString, cVeinModuleParameter*> m_Parameter2ValidateHash;
};

#endif // MODULEVALIDATOR_H
