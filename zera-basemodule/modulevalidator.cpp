#include <QEvent>
#include <QHash>

#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>
#include <vcmp_remoteproceduredata.h>
#include <ve_eventdata.h>
#include <ve_commandevent.h>
#include <ve_storagesystem.h>

#include "basemodule.h"
#include "modulevalidator.h"
#include "veinmoduleparameter.h"

cModuleValidator::cModuleValidator(cBaseModule* module)
    : m_pModule(module), Zera::CommandValidator()
{
}


void cModuleValidator::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{
    // is it an event for our module
    if (t_cEvent->eventData()->entityId() == m_pModule->m_nEntityId)
    {
        // is it a command event for setting component data
        if (t_cEvent->eventData()->type() == VeinComponent::ComponentData::dataType())

        {
            // only transactions will be handled
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (t_cEvent->eventData());
            if (t_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::TRANSACTION)
            {
                QString cName;
                // does this component data belong to our module
                if (m_keylist.indexOf(cName = cData->componentName()) != -1 )
                {
                    // we only take new values if the old values are equal
                    if (cData->oldValue() == m_pModule->m_pStorageSystem->getStoredValue(m_pModule->m_nEntityId, cName))
                    {
                        cVeinModuleParameter *param;

                        param = m_Parameter2ValidateHash[cName];
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
                if(m_pModule->veinModuleRpcList.contains(rpcData->procedureName())) {
                    const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                    Q_ASSERT(callId.isNull() == false);
                    m_pModule->veinModuleRpcList[rpcData->procedureName()]->callFunction(callId,t_cEvent->peerId(),rpcData->invokationData());
                    t_cEvent->accept();
                }
                else { //unknown procedure TBD: error event
                    qWarning() << "No remote procedure with entityId:" << m_pModule->m_nEntityId << "name:" << rpcData->procedureName();
                    VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(m_pModule->m_nEntityId).arg(rpcData->procedureName())));
                    VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                    eData->setEntityId(m_pModule->m_nEntityId);
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


/*

                        if (cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_FETCH)
                        {
                            if (m_Parameter2ValidateHash[cName]->hasDeferredNotification())
                            {

                            }
                            else



                        }

                        QVariant newval;

                        newval = cData->newValue();

                        // is newValue a valid parameter
                        if (param->isValidParameter(newval))
                        {
                            cData->setNewValue(newval);
                            if (m_Parameter2ValidateHash[cName]->hasDeferredNotification())
                            {
                                // in case of deferred notification the module will send a command event
                                // or perhaps an error event for notification after all work is done

                                param->transaction(t_cEvent->peerId(), newval);
                                t_cEvent->accept(); // so we accept the command event here
                            }
                            else
                            {
                                param->transaction(newval); // otherwise we "fire and forget" the new value

                                // and convert the command events transaction into notification
                                t_cEvent->setEventSubtype(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION);
                                t_cEvent->eventData()->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                                t_cEvent->eventData()->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                            }

                        }

                        else
                        {
                            t_cEvent->accept();

                            VeinComponent::ErrorData *errData = new VeinComponent::ErrorData();

                            errData->setEntityId(m_pModule->m_nEntityId);
                            errData->setOriginalData(t_cEvent->eventData());
                            errData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                            errData->setEventTarget(t_cEvent->eventData()->eventTarget());
                            errData->setErrorDescription("");

                            VeinEvent::CommandEvent *cEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, errData);
                            emit sigSendEvent(cEvent);

                         }

                    }

                    else
                        t_cEvent->accept();
                }
            }
        }
    }
}
*/

void cModuleValidator::setParameterHash(QHash<QString, cVeinModuleParameter *> &parameterhash)
{
    m_Parameter2ValidateHash = parameterhash;
    m_keylist = m_Parameter2ValidateHash.keys();
}

