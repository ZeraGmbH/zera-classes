#include "scpirpcdelegate.h"
#include <zscpi_response_definitions.h>
#include <vf_server_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <vcmp_remoteproceduredata.h>
#include <QRegularExpression>

SCPIMODULE::cSCPIRpcDelegate::cSCPIRpcDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule *scpimodule, cSCPICmdInfoPtr scpicmdinfo) :
    ScpiBaseDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_scpicmdinfo(scpicmdinfo)
{
}

void SCPIMODULE::cSCPIRpcDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = sInput;
    QString RPC = sInput.section(':', -1);
    int paramsNumber = extractRpcParams(RPC).count();
    bool bQuery = cmd.isQuery() || cmd.isQuery(paramsNumber); // we allow queries without or with 1 parameter
    if ( (bQuery && ((scpiCmdType & SCPI::isQuery) > 0))) // test if we got an allowed query
    {
        if(isCommandRPC(sInput)) {
            executeScpiRpc(client, sInput, cmd);
        }
    }
    else
        client->receiveStatus(ZSCPI::nak);
}

void SCPIMODULE::cSCPIRpcDelegate::executeScpiRpc(cSCPIClient *client, QString &sInput, cSCPICommand cmd)
{
    for(QString RpcCmd: m_pModule->getRpcCmdData().keys()) {
        if(sInput.contains(RpcCmd, Qt::CaseInsensitive)) {
            VfRPCInvokerPtr rpcInvoker = VfRPCInvoker::create(m_scpicmdinfo->entityId, std::make_unique<VfServerRPCInvoker>()); // will client rpc invoker work ?
            connect(rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished, this, [=](bool ok, QUuid identifier, const QVariantMap &resultData) {
                QVariant returnData;
                bool rpcSuccessful = (resultData[VeinComponent::RemoteProcedureData::s_resultCodeString] == VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);
                if(rpcSuccessful)
                    returnData = resultData[VeinComponent::RemoteProcedureData::s_returnString];
                else
                    returnData = resultData[VeinComponent::RemoteProcedureData::s_errorMessageString];
                client->receiveAnswer(returnData.toString(), true);
            });
            m_pModule->getCmdEventHandlerSystem()->addItem(rpcInvoker);

            QString RPC = sInput.section(':', -1);
            QString RPCName = extractRpcName(RPC);
            QStringList parameterNames = extractRpcParams(RPC);
            QStringList parameterValues = cmd.getParamList();
            QStringList parameterTypes = m_pModule->getRpcCmdData().value(RpcCmd).split(",");
            QVariantMap params;

            if(parameterValues.count() != parameterTypes.count() || parameterNames.count() != parameterValues.count()) {
                //QAssert()
            }
            for(int i=0; i<parameterValues.count(); i++) {
                QVariant variantValue = convertParamStrToType(parameterValues[i], parameterTypes[i]);
                params[parameterNames[i]] = variantValue;
            }
            rpcInvoker->invokeRPC(RPCName, params);
        }
    }
}

bool SCPIMODULE::cSCPIRpcDelegate::isCommandRPC(QString &sInput)
{
    bool found = false;
    for(QString RpcCmd: m_pModule->getRpcCmdData().keys())
        if(sInput.contains(RpcCmd, Qt::CaseInsensitive))
            found = true;
    return found;
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
