#include "scpiparameterdelegate.h"
#include "scpieventsystem.h"
#include "scpiclient.h"
#include <scpicommand.h>
#include <vf_server_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <zscpi_response_definitions.h>
#include <vcmp_componentdata.h>
#include <vcmp_remoteproceduredata.h>
#include <QRegularExpression>

namespace SCPIMODULE {

cSCPIParameterDelegate::cSCPIParameterDelegate(QString cmdParent,
                                               QString cmd,
                                               quint8 type,
                                               cSCPIModule *scpimodule,
                                               cSCPICmdInfoPtr scpicmdinfo) :
    ScpiBaseDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_pSCPICmdInfo(scpicmdinfo)
{
}

void cSCPIParameterDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = sInput;
    bool bQuery = cmd.isQuery() || cmd.isQuery(1); // we allow queries without or with 1 parameter
    if ( (bQuery && ((scpiCmdType & SCPI::isQuery) > 0)) ||  // test if we got an allowed query
         (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) ||  // test if we got an allowed cmd + 1 parameter
         ((scpiCmdType & SCPI::isXMLCmd) > 0) ) // test if we expext an xml command
    {
        if(isCommandRPC(sInput)) {
            executeScpiRpc(client, sInput, cmd);
        }
        else {
            VeinComponent::ComponentData *cData;
            cData = new VeinComponent::ComponentData();
            cData->setEntityId(m_pSCPICmdInfo->entityId);
            cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
            cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
            cData->setComponentName(m_pSCPICmdInfo->componentName);
            cData->setOldValue(m_pModule->getStorageDb()->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentName));
            if (bQuery) {
                if (cmd.isQuery(1))
                    cData->setNewValue(cmd.getParam(0));
                cData->setCommand(VeinComponent::ComponentData::Command::CCMD_FETCH);
            }
            else {
                if ((scpiCmdType & SCPI::isXMLCmd) > 0)
                    cData->setNewValue(cmd.getParam()); // if we expect an xml command we take all text behind the command
                else
                    cData->setNewValue(cmd.getParam(0));
                cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
            }
            VeinEvent::CommandEvent *event;
            event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
            event->setPeerId(client->getClientId());

            // we memorize : for component (componentname) the client to set something
            SCPIClientInfoPtr clientinfo;
            if (bQuery)
                clientinfo = std::make_shared<cSCPIClientInfo>(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parQuery);
            else
                clientinfo = std::make_shared<cSCPIClientInfo>(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parcmd);

            m_pModule->scpiParameterCmdInfoHash.insert(m_pSCPICmdInfo->componentName, clientinfo);
            client->addSCPIClientInfo(m_pSCPICmdInfo->componentName, clientinfo);

            m_pModule->m_pSCPIEventSystem->sigSendEvent(event);
        }
    }
    else
        client->receiveStatus(ZSCPI::nak);
}

void cSCPIParameterDelegate::executeScpiRpc(cSCPIClient *client, QString &sInput, cSCPICommand cmd)
{
    for(QString RpcCmd: m_pModule->getRpcCmdData().keys()) {
        if(sInput.contains(RpcCmd, Qt::CaseInsensitive)) {
            VfRPCInvokerPtr rpcInvoker = VfRPCInvoker::create(m_pSCPICmdInfo->entityId, std::make_unique<VfServerRPCInvoker>()); // will client rpc invoker work ?
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

bool cSCPIParameterDelegate::isCommandRPC(QString &sInput)
{
    bool found = false;
    for(QString RpcCmd: m_pModule->getRpcCmdData().keys())
        if(sInput.contains(RpcCmd, Qt::CaseInsensitive))
            found = true;
    return found;
}

QVariant cSCPIParameterDelegate::convertParamStrToType(QString parameter, QString type)
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

QStringList cSCPIParameterDelegate::extractRpcParams(QString RPC)
{
    // RPC_name(type p_paramName1, type p_paramName2)
    QString params;
    QRegularExpression re("\\((.*)\\)");
    QRegularExpressionMatch match = re.match(RPC);

    if (match.hasMatch())
        params = match.captured(1).trimmed();
    return params.split(",", Qt::SkipEmptyParts);
}

QString cSCPIParameterDelegate::extractRpcName(QString RPC)
{
    QString RPCName = RPC.section('(', 0, 0);
    return RPCName;
}

}
