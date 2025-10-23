#include "vfmodulerpcsimplified.h"
#include <vcmp_errordata.h>
#include <vcmp_remoteproceduredata.h>
#include <ve_eventdata.h>

VfModuleRpcSimplified::VfModuleRpcSimplified(int entityId):
    VfEventSystemCommandFilter(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION), // or transaction
    m_entityId(entityId)
{}

void VfModuleRpcSimplified::processCommandEvent(VeinEvent::CommandEvent *commandEvent)
{
    if(commandEvent->eventData()->entityId() == m_entityId &&
        commandEvent->eventData()->type() == VeinComponent::RemoteProcedureData::dataType()) {
        VeinComponent::RemoteProcedureData *rpcData = static_cast<VeinComponent::RemoteProcedureData *>(commandEvent->eventData());
        bool rpcFound = false;
        QString procedureName;
        if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL) {
            for (auto it = m_rpcHash.constBegin(); it != m_rpcHash.constEnd(); ++it) {
                procedureName = it.key();
                if (procedureName.contains(rpcData->procedureName(), Qt::CaseInsensitive)) {
                    rpcFound = true;
                    break ;
                }
            }
            if(rpcFound) {
                const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                Q_ASSERT(!callId.isNull());
                m_rpcHash[procedureName]->callFunction(callId,
                                                       commandEvent->peerId(),
                                                       rpcData->invokationData());
                commandEvent->accept();
            }
            else {
                VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(m_entityId).arg(procedureName)));
                VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                eData->setEntityId(m_entityId);
                eData->setErrorDescription(QString("No remote procedure with name: %1").arg(procedureName));
                eData->setOriginalData(rpcData);
                eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
                errorEvent->setPeerId(commandEvent->peerId());
                commandEvent->accept();
                emit sigSendEvent(errorEvent);
            }
        }
    }

}

void VfModuleRpcSimplified::addRpc(VfCpp::VfCppRpcSimplifiedPtr rpc)
{
    m_rpcHash[rpc->getSignature()] = rpc;
}
