#ifndef VFEVENTSYTEMMODULEPARAM_H
#define VFEVENTSYTEMMODULEPARAM_H

#include "vfeventsystemcommandfilter.h"
#include "vfmoduleparameter.h"
#include <vs_abstracteventsystem.h>
#include <QHash>

class VfEventSytemModuleParam : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfEventSytemModuleParam(int entityId, const VeinStorage::AbstractEventSystem* storageSystem);
    ~VfEventSytemModuleParam();
    virtual void processCommandEvent(VeinEvent::CommandEvent *commandEvent);
    void setParameterMap(const QMap<QString, VfModuleParameter *> &parameterHash);
    static int getInstanceCount();
private:
    int m_entityId;
    const VeinStorage::AbstractEventSystem* m_storageSystem;
    QMap<QString, VfModuleParameter*> m_parameterHash;
    static int m_instanceCount;
};

#endif // VFEVENTSYTEMMODULEPARAM_H
