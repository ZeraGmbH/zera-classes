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

cSCPIParameterDelegate::cSCPIParameterDelegate(const QString &cmdParent,
                                               const QString &cmd,
                                               quint8 type,
                                               cSCPIModule *scpimodule,
                                               cSCPICmdInfoPtr scpicmdinfo) :
    ScpiBaseDelegate(cmdParent, cmd, type),
    m_pModule(scpimodule),
    m_pSCPICmdInfo(scpicmdinfo)
{
}

void cSCPIParameterDelegate::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = scpi;
    bool bQuery = cmd.isQuery() || cmd.isQuery(1); // we allow queries without or with 1 parameter
    if ( (bQuery && ((scpiCmdType & SCPI::isQuery) > 0)) ||  // test if we got an allowed query
         (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) ||  // test if we got an allowed cmd + 1 parameter
         ((scpiCmdType & SCPI::isXMLCmd) > 0) ) // test if we expext an xml command
    {
        if (handleFutureComponent(client, bQuery, scpiTransactionId))
            return;

        VeinComponent::ComponentData *cData = new VeinComponent::ComponentData();
        cData->setEntityId(m_pSCPICmdInfo->entityId);
        cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        cData->setComponentName(m_pSCPICmdInfo->componentOrRpcName);
        cData->setOldValue(m_pModule->getStorageDb()->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentOrRpcName));
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

        SCPIVeinTransactionInfoPtr transactionInfo;
        if (bQuery)
            transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parQuery, scpiTransactionId);
        else
            transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parcmd, scpiTransactionId);

        m_pModule->scpiParameterCmdInfoHash.insert(m_pSCPICmdInfo->componentOrRpcName, transactionInfo);
        client->addSCPITransactionInfo(m_pSCPICmdInfo->componentOrRpcName, transactionInfo);

       emit m_pModule->m_pSCPIEventSystem->sigSendEvent(event);
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

bool cSCPIParameterDelegate::handleFutureComponent(cSCPIClient *client, bool bQuery, const ScpiTransactionId &scpiTransactionId)
{
    VeinStorage::AbstractDatabase *storrageDb = m_pModule->getStorageDb();
    const VeinStorage::AbstractComponentPtr futureComponent = storrageDb->findFutureComponent(m_pSCPICmdInfo->entityId,
                                                                                              m_pSCPICmdInfo->componentOrRpcName);
    if (futureComponent) {
        if (bQuery)
            client->handleCmdFinish(futureComponent->getValue().toString(), scpiTransactionId);
        else
            client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
        return true;
    }
    return false;
}

}
