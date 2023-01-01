#include "modulevalidator.h"
#include <vcmp_remoteproceduredata.h>
#include <vcmp_errordata.h>

ModuleValidator::ModuleValidator(int entityId,
                                 VeinEvent::StorageSystem *storageSystem,
                                 QHash<QString, VfCpp::cVeinModuleRpc::Ptr> *veinModuleRpcList) :
    m_entityId(entityId),
    m_storageSystem(storageSystem),
    m_veinModuleRpcList(veinModuleRpcList)
{
}

void ModuleValidator::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{
    if (t_cEvent->eventData()->entityId() == m_entityId) {
        // is it a command event for setting component data
        if (t_cEvent->eventData()->type() == VeinComponent::ComponentData::dataType()) {
            // only transactions will be handled
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (t_cEvent->eventData());
            if (t_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::TRANSACTION) {
                QString cName = cData->componentName();
                // does this component data belong to our module
                auto hashIter = m_Parameter2ValidateHash.find(cName);
                if(hashIter != m_Parameter2ValidateHash.end()) {
                    // we only take new values if the old values are equal
                    if (cData->oldValue() == m_storageSystem->getStoredValue(m_entityId, cName)) {
                        cVeinModuleParameter *param = hashIter.value();
                        param->transaction(t_cEvent->peerId(), cData->newValue(), cData->oldValue(), cData->eventCommand());
                        t_cEvent->accept(); // it is an event for us ... the parameter will do the rest
                    }
                }
            }
        }
        // handle rpcs
        else if(t_cEvent->eventData()->type() == VeinComponent::RemoteProcedureData::dataType()) {
            VeinComponent::RemoteProcedureData *rpcData=nullptr;
            rpcData = static_cast<VeinComponent::RemoteProcedureData *>(t_cEvent->eventData());
            if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL) {
                if(m_veinModuleRpcList->contains(rpcData->procedureName())) {
                    const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                    Q_ASSERT(callId.isNull() == false);
                    (*m_veinModuleRpcList)[rpcData->procedureName()]->callFunction(callId,t_cEvent->peerId(),rpcData->invokationData());
                    t_cEvent->accept();
                }
                else {
                    qWarning() << "No remote procedure with entityId:" << m_entityId << "name:" << rpcData->procedureName();
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
}

void ModuleValidator::setParameterHash(QHash<QString, cVeinModuleParameter *> &parameterhash)
{
    m_Parameter2ValidateHash = parameterhash;
}
