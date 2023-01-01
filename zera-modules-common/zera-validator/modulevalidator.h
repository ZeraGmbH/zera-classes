#ifndef MODULEVALIDATOR_H
#define MODULEVALIDATOR_H

#include <veincommandfiltereventsystem.h>
#include <veinmoduleparameter.h>
#include <ve_storagesystem.h>
#include <vf-cpp-rpc.h>
#include <QHash>

class ModuleValidator : public VeinCommandFilterEventSystem
{
    Q_OBJECT
public:
    ModuleValidator(int entityId,
                    VeinEvent::StorageSystem* storageSystem,
                    QHash<QString, VfCpp::cVeinModuleRpc::Ptr> *veinModuleRpcList);
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
    void setParameterHash(QHash<QString, cVeinModuleParameter *>&parameterhash);
private:
    int m_entityId;
    VeinEvent::StorageSystem* m_storageSystem;
    QHash<QString, cVeinModuleParameter*> m_Parameter2ValidateHash;
    QHash<QString, VfCpp::cVeinModuleRpc::Ptr> *m_veinModuleRpcList;
};

#endif // MODULEVALIDATOR_H
