#include "vfmodulerpc.h"
#include <QJsonObject>

VfModuleRpc::VfModuleRpc(VfCpp::VfCppRpcSimplifiedPtr rpc, const QString &description) :
    m_rpc(rpc),
    m_description(description),
    m_pValidator(nullptr)
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

void VfModuleRpc::exportRpcSCPIInfo(QJsonArray &jsArr) const
{
    if (m_rpcScpiInfo)
        m_rpcScpiInfo->appendSCPIInfo(jsArr);
}

void VfModuleRpc::exportMetaData(QJsonObject &jsObj) const
{
    QJsonObject rpcInfo;
    rpcInfo.insert("Description", m_description);
    if (m_pValidator != nullptr) {
        QJsonObject jsonObj2;
        m_pValidator->exportMetaData(jsonObj2);
        rpcInfo.insert("Validation", jsonObj2);
    }
    jsObj.insert(m_rpc->getSignature(), rpcInfo);
}

void VfModuleRpc::setValidator(ValidatorInterface *validator)
{
    if(m_pValidator)
        delete m_pValidator;
    m_pValidator = validator;
}
