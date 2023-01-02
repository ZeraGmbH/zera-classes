#ifndef VFMODULERPC_H
#define VFMODULERPC_H

#include <vfcommandfiltereventsystem.h>
#include <vf-cpp-rpc.h>
#include <QHash>

class VfModuleRpc : public VeinCommandFilterEventSystem
{
    Q_OBJECT
public:
    VfModuleRpc(int entityId);
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
    void addRpc(VfCpp::cVeinModuleRpc::Ptr rpc);
private:
    int m_entityId;
    QHash<QString, VfCpp::cVeinModuleRpc::Ptr> m_rpcHash;
};

#endif // VFMODULERPC_H
