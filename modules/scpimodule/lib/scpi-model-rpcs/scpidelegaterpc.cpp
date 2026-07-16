#include "scpidelegaterpc.h"
#include "scpimodulecommonstaticfunctions.h"
#include <zscpi_response_definitions.h>
#include <vf_client_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <vcmp_remoteproceduredata.h>
#include <vf-cpp-rpc-helper.h>
#include <QRegularExpression>

namespace SCPIMODULE {

ScpiDelegateRpc::ScpiDelegateRpc(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiQueryCmdFlags),
    m_pModule(params.scpimodule),
    m_entityId(params.entityId),
    m_rpcSignature(params.rpcSignature),
    m_veinComponentInfo(params.veinComponentInfo)
{
}

void ScpiDelegateRpc::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    quint8 scpiQueryCmdFlags = getType();
    cSCPICommand cmd = scpi;

    bool bQuery = ScpiModuleCommonStaticFunctions::isQuery(scpi);
    bool bCmd = cmd.isCommand(1) && cmd.getParamList()[0].isEmpty();
    bool bCmdwP = cmd.isCommand(1) || m_veinComponentInfo.contains("Optional parameter");

    if ((bQuery && ((scpiQueryCmdFlags & SCPI::isQuery) > 0)) ||
        (bCmd && ((scpiQueryCmdFlags & SCPI::isCmd) >  0)) ||
        (bCmdwP && ((scpiQueryCmdFlags & SCPI::isCmdwP) >  0)))
        executeScpiRpc(client, scpi, bQuery, scpiTransactionId);
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiDelegateRpc::handleRpcFinish(const QVariantMap &resultData, const SCPIVeinTransactionInfoPtr &transactionInfo, bool inputIsQuery)
{
    QMetaObject::Connection myConn = connect(this, &ScpiDelegateRpc::sigClientInfoSignal,
                                             transactionInfo->getClient(), &cSCPIClient::removeVeinParamRpcTransactionInfo, Qt::QueuedConnection);
    emit sigClientInfoSignal(m_rpcSignature);
    disconnect(myConn);

    m_pModule->removeScpiVeinParamRpcTransaction(m_rpcSignature, transactionInfo);

    bool rpcSuccessful = (resultData[VeinComponent::RemoteProcedureData::s_resultCodeString] == VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);
    if(inputIsQuery) {
        QVariant returnData;
        if(rpcSuccessful)
            returnData = resultData[VeinComponent::RemoteProcedureData::s_returnString];
        else
            returnData = resultData[VeinComponent::RemoteProcedureData::s_errorMessageString];
        transactionInfo->getClient()->handleCmdFinish(returnData.toString(), transactionInfo->getScpiTransactionId());
    }
    else {//in case of command
        if(rpcSuccessful)
            transactionInfo->getClient()->handleCmdFinishStatusOnly(ZSCPI::ack, transactionInfo->getScpiTransactionId());
        else
            transactionInfo->getClient()->handleCmdFinishStatusOnly(ZSCPI::errexec, transactionInfo->getScpiTransactionId());
    }
}

void ScpiDelegateRpc::executeScpiRpc(cSCPIClient *client, const QString &scpi, bool inputIsQuery, const ScpiTransactionId &scpiTransactionId)
{
    SCPIVeinTransactionInfoPtr transactionInfo;
    if (inputIsQuery)
        transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_entityId, parQuery, scpiTransactionId);
    else
        transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_entityId, parcmd, scpiTransactionId);

    VfRPCInvokerPtr rpcInvoker = VfRPCInvoker::create(m_entityId, std::make_unique<VfClientRPCInvoker>());
    connect(rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished, this, [=](bool ok, const QVariantMap &resultData) {
        Q_UNUSED(ok)
        handleRpcFinish(resultData, transactionInfo, inputIsQuery);
    });
    m_pModule->getCmdEventHandlerSystem()->addItem(rpcInvoker);

    const QString rpcSignature = m_rpcSignature;
    QString rpcName = VfCppRpcHelper::getRpcName(rpcSignature);
    const QStringList paramNamesList = VfCppRpcHelper::getRpcParamNamesList(rpcSignature);
    int totalExpectedParams = paramNamesList.size();
    QStringList paramTypesList = VfCppRpcHelper::getRpcTypesListFromSignature(rpcSignature);

    cSCPICommand cmd = scpi;
    QStringList scpiCmdParams = cmd.getParamList();

    if((totalExpectedParams > scpiCmdParams.size()) && (m_veinComponentInfo.contains("Optional parameter")))
        scpiCmdParams.append("");

    if(totalExpectedParams == scpiCmdParams.size()) {
        QVariantMap params;
        for(int i=0; i<scpiCmdParams.size(); i++) {
            QVariant variantValue = convertParamStrToType(scpiCmdParams[i], paramTypesList[i]);
            params[paramNamesList[i]] = variantValue;
        }

        m_pModule->insertScpiVeinParamRpcTransaction(rpcSignature, transactionInfo);
        client->addVeinParamRpcTransactionInfo(rpcSignature, transactionInfo);
        rpcInvoker->invokeRPC(rpcName, params);
    }

    else if(cmd.getParamList()[0].isEmpty() && totalExpectedParams == 0) {
        rpcInvoker->invokeRPC(rpcName, QVariantMap());
    }
}

QVariant ScpiDelegateRpc::convertParamStrToType(const QString &parameter, const QString &type)
{
    if (type == "int")
        return parameter.toInt();
    else if (type == "double")
        return parameter.toDouble();
    else if (type == "float")
        return parameter.toFloat();
    else if (type == "bool") {
        QString entry = parameter.trimmed().toLower();
        if(entry == "true" || entry == "1")
            return true;
        else if(entry == "false" || entry== "0")
            return false;
        else
            return QVariant();
    }
    else if (type == "QString")
        return parameter;
    return QVariant();
}

}
