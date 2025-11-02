#ifndef VFRPCEVENTSYSTEMSIMPLIFIED_H
#define VFRPCEVENTSYSTEMSIMPLIFIED_H

#include <vf-cpp-rpc-simplified.h>
#include <vfeventsystemcommandfilter.h>

class VfRpcEventSystemSimplified : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfRpcEventSystemSimplified(int entityId);
    virtual void processCommandEvent(VeinEvent::CommandEvent *commandEvent);
    void addRpc(VfCpp::VfCppRpcSimplifiedPtr rpc);
private:
    int m_entityId;
    QHash<QString, VfCpp::VfCppRpcSimplifiedPtr> m_rpcHash;
};

#endif // VFRPCEVENTSYSTEMSIMPLIFIED_H
