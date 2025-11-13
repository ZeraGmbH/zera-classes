#include "vfrpceventsystemsimplified.h"
#include <vcmp_errordata.h>
#include <vcmp_remoteproceduredata.h>
#include <ve_eventdata.h>

VfRpcEventSystemSimplified::VfRpcEventSystemSimplified(int entityId):
    VfEventSystemCommandFilter(VeinEvent::CommandEvent::EventSubtype::TRANSACTION),
    m_entityId(entityId)
{}

void VfRpcEventSystemSimplified::processCommandEvent(VeinEvent::CommandEvent *commandEvent)
{
    if(commandEvent->eventData()->entityId() == m_entityId &&
        commandEvent->eventData()->type() == VeinComponent::RemoteProcedureData::dataType()) {
        VeinComponent::RemoteProcedureData *rpcData = static_cast<VeinComponent::RemoteProcedureData *>(commandEvent->eventData());
        if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL) {
            if(m_veinModuleRPCMap.contains(rpcData->procedureName())) {
                const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                Q_ASSERT(!callId.isNull());
                auto rpcParam = m_veinModuleRPCMap.value(rpcData->procedureName());

                if(rpcParam->isValidParameter(rpcData->invokationData())) {
                    m_veinModuleRPCMap[rpcData->procedureName()]->getRpcSimplifed()->callFunction(callId,
                                                                                                  commandEvent->peerId(),
                                                                                                  rpcData->invokationData());
                    commandEvent->accept();
                }
                else
                    sendErrorEvent(QString("Invalid parameter %1").arg(rpcData->procedureName()), commandEvent);
            }
            else {
                VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(m_entityId).arg(rpcData->procedureName())));
                sendErrorEvent(QString("No remote procedure with name: %1").arg(rpcData->procedureName()), commandEvent);
            }
        }
    }
}

void VfRpcEventSystemSimplified::setRPCMap(QMap<QString, VfModuleRpcPtr> veinModuleRPCMap)
{
    m_veinModuleRPCMap = veinModuleRPCMap;
}

void VfRpcEventSystemSimplified::sendErrorEvent(QString errorMsg, VeinEvent::CommandEvent *commandEvent)
{
    VeinComponent::RemoteProcedureData *rpcData = static_cast<VeinComponent::RemoteProcedureData *>(commandEvent->eventData());

    VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
    eData->setEntityId(m_entityId);
    eData->setErrorDescription(errorMsg);
    eData->setOriginalData(rpcData);
    eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);

    VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    errorEvent->setPeerId(commandEvent->peerId());
    commandEvent->accept();
    emit sigSendEvent(errorEvent);
}
