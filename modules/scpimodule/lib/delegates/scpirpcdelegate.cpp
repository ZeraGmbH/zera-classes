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
    QStringList scpiExpectedParameters = extractRpcParams(m_scpicmdinfo->componentOrRpcName);
    int scpiExpectedParametersCount = scpiExpectedParameters.count();

    bool bQuery = cmd.isQuery() || cmd.isQuery(scpiExpectedParametersCount);
    bool bCmd = cmd.isCommand();
    bool bCmdwP = cmd.isCommand(scpiExpectedParametersCount) || m_scpicmdinfo->veinComponentInfo.contains("Optional parameter");

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
    connect(rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished, this, [=](bool ok, QUuid identifier, const QVariantMap &resultData) {
        Q_UNUSED(ok)
        Q_UNUSED(identifier)

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
                client->receiveStatus(ZSCPI::nak);
        }
    });
    m_pModule->getCmdEventHandlerSystem()->addItem(rpcInvoker);

    QString rpcSignature = m_scpicmdinfo->componentOrRpcName;
    QString rpcName = extractRpcName(rpcSignature);
    const QStringList parameterList = extractRpcParams(rpcSignature);
    QStringList parameterNames;
    for (const QString &parameter : parameterList) {
        QStringList paramTypeValue = parameter.split(" ");
        if (paramTypeValue.size() == 2)
            parameterNames.append(paramTypeValue[1]);
    }

    cSCPICommand cmd = sInput;
    QStringList parameterValues = cmd.getParamList();
    QStringList parameterTypes = VfCppRpcHelper::getRpcTypesListFromSignature(rpcSignature);

    if(parameterNames.size() != parameterValues.size())
        if(m_scpicmdinfo->veinComponentInfo.contains("Optional parameter"))
            parameterValues.append("");

    QVariantMap params;
    for(int i=0; i<parameterValues.count(); i++) {
        QVariant variantValue = convertParamStrToType(parameterValues[i], parameterTypes[i]);
        params[parameterNames[i]] = variantValue;
    }


    m_pModule->scpiParameterCmdInfoHash.insert(m_scpicmdinfo->componentOrRpcName, clientinfo);
    client->addSCPIClientInfo(m_scpicmdinfo->componentOrRpcName, clientinfo);

    rpcInvoker->invokeRPC(rpcName, params);
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

QStringList SCPIMODULE::cSCPIRpcDelegate::extractRpcParams(QString RPC)
{
    // RPC_name(type p_paramName1, type p_paramName2)
    QString params;
    QRegularExpression re("\\((.*)\\)");
    QRegularExpressionMatch match = re.match(RPC);

    if (match.hasMatch())
        params = match.captured(1).trimmed();
    return params.split(",", Qt::SkipEmptyParts);
}

QString SCPIMODULE::cSCPIRpcDelegate::extractRpcName(QString RPC)
{
    QString RPCName = RPC.section('(', 0, 0);
    return RPCName;
}
