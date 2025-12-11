#include "vfmodulerpc.h"
#include <vf-cpp-rpc-helper.h>
#include <vcmp_remoteproceduredata.h>
#include <QJsonObject>

VfModuleRpc::VfModuleRpc(VfCpp::VfCppRpcSimplifiedPtr rpc, const QString &description) :
    m_rpc(rpc),
    m_description(description),
    m_pValidator(nullptr),
    m_optParam(false)
{
    int totalExpectedParams = VfCppRpcHelper::getRpcParamNamesList(rpc->getSignature()).size();
}

VfCpp::VfCppRpcSimplifiedPtr VfModuleRpc::getRpcSimplifed()
{
    return m_rpc;
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
    if(m_optParam)
        rpcInfo.insert("Optional parameter", "");
    jsObj.insert(m_rpc->getSignature(), rpcInfo);
}

void VfModuleRpc::setValidator(ValidatorInterface *validator)
{
    if(m_pValidator)
        delete m_pValidator;
    m_pValidator = validator;
}

void VfModuleRpc::canAcceptOptionalParam()
{
    m_optParam = true;
}

bool VfModuleRpc::isValidParameter(QVariantMap invokationData)
{
    //We handle only one param for now
    if (m_pValidator != 0) {
        QVariantMap paramMap = invokationData.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
        if(!paramMap.isEmpty()) {
            QVariant input = paramMap.value(paramMap.firstKey());
            if(m_optParam && input == "")
                return true;
            if(m_pValidator->isValidParam(input))
                return true;
            return false;
        }
    }
    return true;
}
