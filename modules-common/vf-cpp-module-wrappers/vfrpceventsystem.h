#ifndef VFRPCEVENTSYSTEM_H
#define VFRPCEVENTSYSTEM_H

#include <vfeventsystemcommandfilter.h>
#include <vf-cpp-rpc.h>
#include <QHash>

class VfRpcEventSystem : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfRpcEventSystem(int entityId);
    virtual void processCommandEvent(VeinEvent::CommandEvent *commandEvent);
    void addRpc(VfCpp::cVeinModuleRpc::Ptr rpc);
private:
    int m_entityId;
    QHash<QString, VfCpp::cVeinModuleRpc::Ptr> m_rpcHash;
};

#endif // VFRPCEVENTSYSTEM_H
