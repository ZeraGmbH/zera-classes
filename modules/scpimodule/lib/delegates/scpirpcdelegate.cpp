#include "scpirpcdelegate.h"
#include <zscpi_response_definitions.h>
#include <vf_client_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <vcmp_remoteproceduredata.h>
#include <vf-cpp-rpc-helper.h>
#include <QRegularExpression>

SCPIMODULE::cSCPIRpcDelegate::cSCPIRpcDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule *scpimodule, cSCPICmdInfoPtr scpicmdinfo) :
    ScpiBaseDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_scpicmdinfo(scpicmdinfo)
{
}

void SCPIMODULE::cSCPIRpcDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = sInput;

    bool bQuery = cmd.isQuery() || cmd.isQuery(1);
    bool bCmd = cmd.isCommand();
    bool bCmdwP = cmd.isCommand(1) || m_scpicmdinfo->veinComponentInfo.contains("Optional parameter");

    if ((bQuery && ((scpiCmdType & SCPI::isQuery) > 0)) ||
        (bCmd && ((scpiCmdType & SCPI::isCmd) >  0)) ||
        (bCmdwP && ((scpiCmdType & SCPI::isCmdwP) >  0)))
        executeScpiRpc(client, sInput, bQuery);
    else
        client->receiveStatus(ZSCPI::nak);
}

void SCPIMODULE::cSCPIRpcDelegate::executeScpiRpc(cSCPIClient *client, QString &sInput, bool inputIsQuery)
{
    SCPIClientInfoPtr clientinfo;
    if (inputIsQuery)
        clientinfo = std::make_shared<cSCPIClientInfo>(client, m_scpicmdinfo->entityId, SCPIMODULE::parQuery);
    else
        clientinfo = std::make_shared<cSCPIClientInfo>(client, m_scpicmdinfo->entityId, SCPIMODULE::parcmd);

    VfRPCInvokerPtr rpcInvoker = VfRPCInvoker::create(m_scpicmdinfo->entityId, std::make_unique<VfClientRPCInvoker>());
    connect(rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished, this, [=](bool ok, const QVariantMap &resultData) {
        Q_UNUSED(ok)

        QMetaObject::Connection myConn = connect(this, &cSCPIRpcDelegate::sigClientInfoSignal,
                                                 clientinfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
        emit sigClientInfoSignal(m_scpicmdinfo->componentOrRpcName);
        disconnect(myConn);

        m_pModule->scpiParameterCmdInfoHash.remove(m_scpicmdinfo->componentOrRpcName, clientinfo);

        QVariant returnData;
        bool rpcSuccessful = (resultData[VeinComponent::RemoteProcedureData::s_resultCodeString] == VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);

        if(inputIsQuery) {
            if(rpcSuccessful)
                returnData = resultData[VeinComponent::RemoteProcedureData::s_returnString];
            else
                returnData = resultData[VeinComponent::RemoteProcedureData::s_errorMessageString];
            client->receiveAnswer(returnData.toString(), true);
        }
        else {//in case of command
            if(rpcSuccessful)
                client->receiveStatus(ZSCPI::ack);
            else
                client->receiveStatus(ZSCPI::errval);
        }
    });
    m_pModule->getCmdEventHandlerSystem()->addItem(rpcInvoker);

    QString rpcSignature = m_scpicmdinfo->componentOrRpcName;
    QString rpcName = VfCppRpcHelper::getRpcName(rpcSignature);
    const QStringList paramNamesList = VfCppRpcHelper::getRpcParamNamesList(rpcSignature);
    int totalExpectedParams = paramNamesList.size();
    QStringList paramTypesList = VfCppRpcHelper::getRpcTypesListFromSignature(rpcSignature);

    cSCPICommand cmd = sInput;
    QStringList scpiCmdParams = cmd.getParamList();
    int totalActualParams = scpiCmdParams.size();

    if((totalExpectedParams > scpiCmdParams.size()) && (m_scpicmdinfo->veinComponentInfo.contains("Optional parameter")))
        scpiCmdParams.append("");

    if(totalExpectedParams == scpiCmdParams.size()) {
        QVariantMap params;
        for(int i=0; i<scpiCmdParams.size(); i++) {
            QVariant variantValue = convertParamStrToType(scpiCmdParams[i], paramTypesList[i]);
            params[paramNamesList[i]] = variantValue;
        }

        m_pModule->scpiParameterCmdInfoHash.insert(m_scpicmdinfo->componentOrRpcName, clientinfo);
        client->addSCPIClientInfo(m_scpicmdinfo->componentOrRpcName, clientinfo);
        rpcInvoker->invokeRPC(rpcName, params);
    }
}

QVariant SCPIMODULE::cSCPIRpcDelegate::convertParamStrToType(QString parameter, QString type)
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
