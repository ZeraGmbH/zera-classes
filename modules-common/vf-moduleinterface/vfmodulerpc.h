#ifndef VFMODULERPC_H
#define VFMODULERPC_H

#include <vf-cpp-rpc-simplified.h>
#include <memory>

class VfModuleRpc
{
public:
    explicit VfModuleRpc(VfCpp::VfCppRpcSimplifiedPtr rpc, const QString& description);
    void setRPCScpiInfo(const QString &model, const QString &cmd,
                        int cmdTypeMask, // e.g SCPI::isQuery|SCPI::isCmdwP
                        const QString &rpcSignature);
    void exportRpcSCPIInfo(QJsonArray &jsArr);

signals:
private:
    VfCpp::VfCppRpcSimplifiedPtr m_rpc;
    const QString m_description;
    std::unique_ptr<VfModuleMetaInfoContainer> m_rpcScpiInfo;
};

typedef std::shared_ptr<VfModuleRpc> VfModuleRpcPtr;

#endif // VFMODULERPC_H
