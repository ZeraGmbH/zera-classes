#ifndef VFMODULERPC_H
#define VFMODULERPC_H

#include <vfeventsystemcommandfilter.h>
#include <vf-cpp-rpc.h>
#include <QHash>

class VfModuleRpc : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfModuleRpc(int entityId);
    virtual void processCommandEvent(VeinEvent::CommandEvent *commandEvent);
    void addRpc(VfCpp::cVeinModuleRpc::Ptr rpc);
private:
    int m_entityId;
    QHash<QString, VfCpp::cVeinModuleRpc::Ptr> m_rpcHash;
};

#endif // VFMODULERPC_H
