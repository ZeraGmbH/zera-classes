#include "scpirpcdelegate.h"
#include "scpimodulecommonstaticfunctions.h"
#include <zscpi_response_definitions.h>
#include <vf_client_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <vcmp_remoteproceduredata.h>
#include <vf-cpp-rpc-helper.h>
#include <QRegularExpression>

SCPIMODULE::cSCPIRpcDelegate::cSCPIRpcDelegate(const QString &cmdParent, const QString &cmd, quint8 scpiCmdQueryFlags, cSCPIModule *scpimodule, cSCPICmdInfoPtr scpicmdinfo) :
    ScpiBaseDelegate(cmdParent, cmd, scpiCmdQueryFlags), m_pModule(scpimodule), m_scpicmdinfo(scpicmdinfo)
{
}

void SCPIMODULE::cSCPIRpcDelegate::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = scpi;

    bool bQuery = ScpiModuleCommonStaticFunctions::isQuery(scpi);
    bool bCmd = cmd.isCommand();
    bool bCmdwP = cmd.isCommand(1) || m_scpicmdinfo->veinComponentInfo.contains("Optional parameter");

    if ((bQuery && ((scpiCmdType & SCPI::isQuery) > 0)) ||
        (bCmd && ((scpiCmdType & SCPI::isCmd) >  0)) ||
        (bCmdwP && ((scpiCmdType & SCPI::isCmdwP) >  0)))
        executeScpiRpc(client, scpi, bQuery, scpiTransactionId);
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void SCPIMODULE::cSCPIRpcDelegate::executeScpiRpc(cSCPIClient *client, const QString &scpi, bool inputIsQuery, const ScpiTransactionId &scpiTransactionId)
{
    SCPIVeinTransactionInfoPtr transactionInfo;
    if (inputIsQuery)
        transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_scpicmdinfo->entityId, SCPIMODULE::parQuery, scpiTransactionId);
    else
        transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_scpicmdinfo->entityId, SCPIMODULE::parcmd, scpiTransactionId);

    VfRPCInvokerPtr rpcInvoker = VfRPCInvoker::create(m_scpicmdinfo->entityId, std::make_unique<VfClientRPCInvoker>());
    connect(rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished, this, [=](bool ok, const QVariantMap &resultData) {
        Q_UNUSED(ok)

        QMetaObject::Connection myConn = connect(this, &cSCPIRpcDelegate::sigClientInfoSignal,
                                                 transactionInfo->getClient(), &cSCPIClient::removeVeinParamRpcTransactionInfo, Qt::QueuedConnection);
        emit sigClientInfoSignal(m_scpicmdinfo->componentOrRpcName);
        disconnect(myConn);

        m_pModule->scpiParameterCmdInfoHash.remove(m_scpicmdinfo->componentOrRpcName, transactionInfo);

        QVariant returnData;
        bool rpcSuccessful = (resultData[VeinComponent::RemoteProcedureData::s_resultCodeString] == VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);

        if(inputIsQuery) {
            if(rpcSuccessful)
                returnData = resultData[VeinComponent::RemoteProcedureData::s_returnString];
            else
                returnData = resultData[VeinComponent::RemoteProcedureData::s_errorMessageString];
            client->handleCmdFinish(returnData.toString(), scpiTransactionId);
        }
        else {//in case of command
            if(rpcSuccessful)
                client->handleCmdFinishStatusOnly(ZSCPI::ack, scpiTransactionId);
            else
                client->handleCmdFinishStatusOnly(ZSCPI::errexec, scpiTransactionId);
        }
    });
    m_pModule->getCmdEventHandlerSystem()->addItem(rpcInvoker);

    QString rpcSignature = m_scpicmdinfo->componentOrRpcName;
    QString rpcName = VfCppRpcHelper::getRpcName(rpcSignature);
    const QStringList paramNamesList = VfCppRpcHelper::getRpcParamNamesList(rpcSignature);
    int totalExpectedParams = paramNamesList.size();
    QStringList paramTypesList = VfCppRpcHelper::getRpcTypesListFromSignature(rpcSignature);

    cSCPICommand cmd = scpi;
    QStringList scpiCmdParams = cmd.getParamList();

    if((totalExpectedParams > scpiCmdParams.size()) && (m_scpicmdinfo->veinComponentInfo.contains("Optional parameter")))
        scpiCmdParams.append("");

    if(totalExpectedParams == scpiCmdParams.size()) {
        QVariantMap params;
        for(int i=0; i<scpiCmdParams.size(); i++) {
            QVariant variantValue = convertParamStrToType(scpiCmdParams[i], paramTypesList[i]);
            params[paramNamesList[i]] = variantValue;
        }

        m_pModule->scpiParameterCmdInfoHash.insert(m_scpicmdinfo->componentOrRpcName, transactionInfo);
        client->addVeinParamRpcTransactionInfo(m_scpicmdinfo->componentOrRpcName, transactionInfo);
        rpcInvoker->invokeRPC(rpcName, params);
    }
}

QVariant SCPIMODULE::cSCPIRpcDelegate::convertParamStrToType(const QString &parameter, const QString &type)
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
