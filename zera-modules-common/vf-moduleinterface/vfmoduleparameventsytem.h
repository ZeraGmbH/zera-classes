#ifndef VFMODULEPARAMEVENTSYTEM_H
#define VFMODULEPARAMEVENTSYTEM_H

#include <vfcommandfiltereventsystem.h>
#include <vfmoduleparameter.h>
#include <ve_storagesystem.h>
#include <QHash>

class VfModuleParamEventSytem : public VfCommandFilterEventSystem
{
    Q_OBJECT
public:
    VfModuleParamEventSytem(int entityId, VeinEvent::StorageSystem* storageSystem);
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
    void setParameterHash(const QHash<QString, VfModuleParameter*> &parameterHash);
private:
    int m_entityId;
    VeinEvent::StorageSystem* m_storageSystem;
    QHash<QString, VfModuleParameter*> m_parameterHash;
};

#endif // VFMODULEPARAMEVENTSYTEM_H
