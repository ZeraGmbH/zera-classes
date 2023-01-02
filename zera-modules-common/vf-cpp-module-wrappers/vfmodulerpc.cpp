#include "vfmodulerpc.h"

VfModuleRpc::VfModuleRpc(int entityId) :
    VfCommandFilterEventSystem(VeinEvent::CommandEvent::EventSubtype::TRANSACTION),
    m_entityId(entityId)
{
}

void VfModuleRpc::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{
    if(t_cEvent->eventData()->entityId() == m_entityId &&
            t_cEvent->eventData()->type() == VeinComponent::RemoteProcedureData::dataType()) {
        VeinComponent::RemoteProcedureData *rpcData = static_cast<VeinComponent::RemoteProcedureData *>(t_cEvent->eventData());
        if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL) {
            if(m_rpcHash.contains(rpcData->procedureName())) {
                const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                Q_ASSERT(!callId.isNull());
                m_rpcHash[rpcData->procedureName()]->callFunction(callId,
                                                                  t_cEvent->peerId(),
                                                                  rpcData->invokationData());
                t_cEvent->accept();
            }
            else {
                VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(m_entityId).arg(rpcData->procedureName())));
                VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                eData->setEntityId(m_entityId);
                eData->setErrorDescription(QString("No remote procedure with name: %1").arg(rpcData->procedureName()));
                eData->setOriginalData(rpcData);
                eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
                errorEvent->setPeerId(t_cEvent->peerId());
                t_cEvent->accept();
                emit sigSendEvent(errorEvent);
            }
        }
    }
}

void VfModuleRpc::addRpc(VfCpp::cVeinModuleRpc::Ptr rpc)
{
    m_rpcHash[rpc->rpcName()] = rpc;
}
