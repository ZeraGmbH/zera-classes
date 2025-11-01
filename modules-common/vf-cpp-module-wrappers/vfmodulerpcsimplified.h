#ifndef VFMODULERPCSIMPLIFIED_H
#define VFMODULERPCSIMPLIFIED_H

#include <vf-cpp-rpc-simplified.h>
#include <vfeventsystemcommandfilter.h>

class VfModuleRpcSimplified : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfModuleRpcSimplified(int entityId);
    virtual void processCommandEvent(VeinEvent::CommandEvent *commandEvent);
    void addRpc(VfCpp::VfCppRpcSimplifiedPtr rpc);
private:
    int m_entityId;
    QHash<QString, VfCpp::VfCppRpcSimplifiedPtr> m_rpcHash;
};

#endif // VFMODULERPCSIMPLIFIED_H
