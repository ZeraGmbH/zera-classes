#include "scpidelegateparameter.h"
#include "scpiclient.h"
#include "scpimodulecommonstaticfunctions.h"
#include <scpicommand.h>
#include <vf_server_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <zscpi_response_definitions.h>
#include <vcmp_componentdata.h>
#include <vcmp_remoteproceduredata.h>
#include <QRegularExpression>

namespace SCPIMODULE {

ScpiDelegateParameter::ScpiDelegateParameter(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiQueryCmdFlags),
    m_pModule(params.scpimodule),
    m_entityId(params.entityId),
    m_componentName(params.componentName)
{
}

void ScpiDelegateParameter::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    quint8 scpiQueryCmdFlags = getType();
    cSCPICommand cmd = scpi;
    bool bQuery = ScpiModuleCommonStaticFunctions::isQuery(scpi);
    if ( (bQuery && ((scpiQueryCmdFlags & SCPI::isQuery) > 0)) ||  // test if we got an allowed query
        (cmd.isCommand(1) && ((scpiQueryCmdFlags & SCPI::isCmdwP) > 0)) ||  // test if we got an allowed cmd + 1 parameter
        ((scpiQueryCmdFlags & SCPI::isXMLCmd) > 0) ) // test if we expext an xml command
    {
        if (handleFutureComponent(client, bQuery, scpiTransactionId))
            return;

        VeinComponent::ComponentData *cData = new VeinComponent::ComponentData();
        cData->setEntityId(m_entityId);
        cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        cData->setComponentName(m_componentName);
        cData->setOldValue(m_pModule->getStorageDb()->getStoredValue(m_entityId, m_componentName));
        if (bQuery) {
            if (cmd.isQuery(1))
                cData->setNewValue(cmd.getParam(0));
            cData->setCommand(VeinComponent::ComponentData::Command::CCMD_FETCH);
        }
        else {
            if ((scpiQueryCmdFlags & SCPI::isXMLCmd) > 0)
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
            transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_entityId, SCPIMODULE::parQuery, scpiTransactionId);
        else
            transactionInfo = std::make_shared<ScpiVeinTransactionInfo>(client, m_entityId, SCPIMODULE::parcmd, scpiTransactionId);

        m_pModule->insertScpiVeinParamRpcTransaction(m_componentName, transactionInfo);
        client->addVeinParamRpcTransactionInfo(m_componentName, transactionInfo);

        m_pModule->emitSigSendEvent(event);
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

bool ScpiDelegateParameter::handleFutureComponent(cSCPIClient *client, bool bQuery, const ScpiTransactionId &scpiTransactionId)
{
    VeinStorage::AbstractDatabase *storrageDb = m_pModule->getStorageDb();
    const VeinStorage::AbstractComponentPtr futureComponent = storrageDb->findFutureComponent(m_entityId,
                                                                                              m_componentName);
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
