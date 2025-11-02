#include "vfmodulerpc.h"

VfModuleRpc::VfModuleRpc(VfCpp::VfCppRpcSimplifiedPtr rpc, const QString &description) :
    m_rpc(rpc),
    m_description(description)
{
}

void VfModuleRpc::setRPCScpiInfo(const QString &model, const QString &cmd, int cmdTypeMask, const QString &rpcSignature)
{
    m_rpcScpiInfo = std::make_unique<VfModuleMetaInfoContainer>(model,
                                                                cmd,
                                                                cmdTypeMask,
                                                                rpcSignature,
                                                                SCPI::isComponent);
}

void VfModuleRpc::exportRpcSCPIInfo(QJsonArray &jsArr)
{
    if (m_rpcScpiInfo)
        m_rpcScpiInfo->appendSCPIInfo(jsArr);
}
