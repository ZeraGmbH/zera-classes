#ifndef VFRPCEVENTSYSTEMSIMPLIFIED_H
#define VFRPCEVENTSYSTEMSIMPLIFIED_H

#include "vfmodulerpc.h"
#include <vf-cpp-rpc-simplified.h>
#include <vfeventsystemcommandfilter.h>

class VfRpcEventSystemSimplified : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfRpcEventSystemSimplified(int entityId);
    virtual void processCommandEvent(VeinEvent::CommandEvent *commandEvent);
    void setRPCMap(QMap<QString, VfModuleRpcPtr> veinModuleRPCMap);
private:
    int m_entityId;
    QMap<QString, VfModuleRpcPtr> m_veinModuleRPCMap;
};

#endif // VFRPCEVENTSYSTEMSIMPLIFIED_H
