#ifndef RPCACTIVATEPUSHBUTTON_H
#define RPCACTIVATEPUSHBUTTON_H

#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <vf-cpp-rpc-simplified.h>
#include <QObject>

class RpcActivatePushButton : public VfCpp::VfCppRpcSimplified
{
    Q_OBJECT
public:
    RpcActivatePushButton(Zera::PcbInterfacePtr pcbInterface, VeinEvent::EventSystem *eventSystem, int entityId);
private slots:
    void callRPCFunction(const QUuid &callId, const QVariantMap &parameters) override;
    void onRpcTaskFinish(bool ok, QUuid rpcCallId);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    TaskContainerInterfacePtr m_activatePushButtonQueue;
};

#endif // RPCACTIVATEPUSHBUTTON_H
