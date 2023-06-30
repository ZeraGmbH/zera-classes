#include "priorityarbitrationsystem.h"

#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <vcmp_entitydata.h>
#include <vcmp_errordata.h>
#include <vcmp_remoteproceduredata.h>

#include <functional>

PriorityArbitrationSystem::PriorityArbitrationSystem(QObject *t_parent) :
    VeinEvent::EventSystem(t_parent),
    m_remoteProcedures({
                       VF_RPC_BIND(arbitrationAdd, std::bind(&PriorityArbitrationSystem::arbitrationAdd, this, std::placeholders::_1, std::placeholders::_2)),
                       VF_RPC_BIND(arbitrationRemove, std::bind(&PriorityArbitrationSystem::arbitrationRemove, this, std::placeholders::_1, std::placeholders::_2))
                       })
{
    connect(this, &PriorityArbitrationSystem::sigAttached, this, &PriorityArbitrationSystem::initECS);
}

bool PriorityArbitrationSystem::processEvent(QEvent *t_event)
{
    bool retVal = false;

    if(t_event->type() == VeinEvent::CommandEvent::eventType())
    {
        VeinEvent::CommandEvent *cEvent = nullptr;
        cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        Q_ASSERT(cEvent != nullptr);
        if(cEvent->eventSubtype() != VeinEvent::CommandEvent::EventSubtype::NOTIFICATION) //we do not need to process notifications
        {
            if(cEvent->eventData()->entityId() == PriorityArbitrationSystem::s_entityId)
            {
                switch(cEvent->eventData()->type())
                {
                case VeinComponent::ComponentData::dataType():
                {
                    VeinComponent::ComponentData *cData=nullptr;
                    cData = static_cast<VeinComponent::ComponentData *>(cEvent->eventData());
                    Q_ASSERT(cData != nullptr);
                    if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET
                            || cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_ADD
                            || cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_REMOVE)
                    {
                        retVal = true;
                        cEvent->accept();
                    }
                    break;
                }
                case VeinComponent::EntityData::dataType():
                {
                    VeinComponent::EntityData *cData=nullptr;
                    cData = static_cast<VeinComponent::EntityData *>(cEvent->eventData());
                    Q_ASSERT(cData != nullptr);
                    if(cData->eventCommand() == VeinComponent::EntityData::Command::ECMD_ADD
                            || cData->eventCommand() == VeinComponent::EntityData::Command::ECMD_REMOVE)
                    {
                        retVal = true;
                        cEvent->accept();
                    }
                    break;
                }
                case VeinComponent::RemoteProcedureData::dataType():
                {
                    VeinComponent::RemoteProcedureData *rpcData=nullptr;
                    rpcData = static_cast<VeinComponent::RemoteProcedureData *>(cEvent->eventData());
                    Q_ASSERT(rpcData != nullptr);
                    if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL
                            && m_remoteProcedures.contains(rpcData->procedureName()))
                    {
                        retVal = true;
                        const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                        Q_ASSERT(callId.isNull() == false);
                        Q_ASSERT(m_pendingRpcHash.contains(callId) == false);
                        m_pendingRpcHash.insert(callId, cEvent->peerId());
                        m_remoteProcedures.value(rpcData->procedureName())(callId, rpcData->invokationData());
                        t_event->accept();
                    }
                    else //unknown procedure
                    {
                        ///@todo refactor redundant entity id specific checks for invalid calls into separate interface
                        retVal = true;
                        qWarning() << "No remote procedure with entityId:" << PriorityArbitrationSystem::s_entityId << "name:" << rpcData->procedureName();
                        VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(PriorityArbitrationSystem::s_entityId).arg(rpcData->procedureName())));
                        VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                        eData->setEntityId(PriorityArbitrationSystem::s_entityId);
                        eData->setErrorDescription(QString("No remote procedure with name: %1").arg(rpcData->procedureName()));
                        eData->setOriginalData(rpcData);
                        eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                        eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                        VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
                        errorEvent->setPeerId(cEvent->peerId());
                        cEvent->accept();
                        emit sigSendEvent(errorEvent);
                    }
                    break;
                }
                }
            }
            else //arbitration checks
            {
                bool validationRequired = false;
                switch(cEvent->eventData()->type())
                {
                case VeinComponent::ComponentData::dataType():
                {
                    VeinComponent::ComponentData *cData=nullptr;
                    cData = static_cast<VeinComponent::ComponentData *>(cEvent->eventData());
                    Q_ASSERT(cData != nullptr);
                    if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_ADD
                            || cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_REMOVE
                            || cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET)
                    {
                        validationRequired = true;
                    }
                    break;
                }
                case VeinComponent::EntityData::dataType(): ///@todo usually no one is adding/removing entities with a transaction so drop this case?
                {
                    VeinComponent::EntityData *eData=nullptr;
                    eData = static_cast<VeinComponent::EntityData *>(cEvent->eventData());
                    Q_ASSERT(eData != nullptr);
                    if(eData->eventCommand() == VeinComponent::EntityData::Command::ECMD_ADD
                            || eData->eventCommand() == VeinComponent::EntityData::Command::ECMD_REMOVE)
                    {
                        validationRequired = true;
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
                if(validationRequired == true)
                {
                    if(m_priorityArbitrationMap.isEmpty() == false
                            && m_priorityArbitrationMap.first() != cEvent->peerId()) //arbitration is in place and the sender has not the highest priority
                    {
                        retVal = true;
                        qWarning() << "Arbitration dropped event:" << cEvent << "from client:" << cEvent->peerId();
                        VeinComponent::ErrorData *errorData = new VeinComponent::ErrorData();
                        errorData->setOriginalData(cEvent->eventData());
                        errorData->setEntityId(cEvent->eventData()->entityId());
                        errorData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                        errorData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                        errorData->setErrorDescription("Arbitration dropped event");

                        VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, errorData);
                        errorEvent->setPeerId(cEvent->peerId());
                        cEvent->accept();
                        emit sigSendEvent(errorEvent);
                    }
                }
            }
        }
    }

    return retVal;
}

void PriorityArbitrationSystem::initECS()
{
    VeinComponent::ComponentData *initData=nullptr;
    VeinComponent::RemoteProcedureData *rpcData=nullptr;

    VeinComponent::EntityData *entityData = new VeinComponent::EntityData();
    entityData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    entityData->setEventOrigin(VeinComponent::EntityData::EventOrigin::EO_LOCAL);
    entityData->setEventTarget(VeinComponent::EntityData::EventTarget::ET_ALL);
    entityData->setEntityId(PriorityArbitrationSystem::s_entityId);
    emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, entityData));
    entityData = nullptr;

    initData = new VeinComponent::ComponentData();
    initData->setEntityId(PriorityArbitrationSystem::s_entityId);
    initData->setCommand(VeinComponent::ComponentData::Command::CCMD_ADD);
    initData->setComponentName(PriorityArbitrationSystem::s_entityNameComponentName);
    initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    initData->setNewValue(PriorityArbitrationSystem::s_entityName);
    emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData));
    initData = nullptr;

    initData = new VeinComponent::ComponentData();
    initData->setEntityId(PriorityArbitrationSystem::s_entityId);
    initData->setCommand(VeinComponent::ComponentData::Command::CCMD_ADD);
    initData->setComponentName(PriorityArbitrationSystem::s_clientPriorityListComponentName);
    initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    initData->setNewValue(QStringList());
    emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData));
    initData = nullptr;



    const QList<QString> tmpRemoteProcedureList = m_remoteProcedures.keys();
    for(const QString &tmpRemoteProcedureName : qAsConst(tmpRemoteProcedureList))
    {
        rpcData = new VeinComponent::RemoteProcedureData();
        rpcData->setEntityId(PriorityArbitrationSystem::s_entityId);
        rpcData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_REGISTER);
        rpcData->setProcedureName(tmpRemoteProcedureName);
        rpcData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        rpcData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);

        emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, rpcData));
    }
}

void PriorityArbitrationSystem::rpcFinished(QUuid t_callId, const QString &t_procedureName, const QVariantMap &t_data)
{
    Q_ASSERT(m_pendingRpcHash.contains(t_callId));
    VeinComponent::RemoteProcedureData *resultData = new VeinComponent::RemoteProcedureData();
    resultData->setEntityId(s_entityId);
    resultData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    resultData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    resultData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_RESULT);
    resultData->setProcedureName(t_procedureName);
    resultData->setInvokationData(t_data);

    VeinEvent::CommandEvent *rpcResultEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, resultData);
    rpcResultEvent->setPeerId(m_pendingRpcHash.value(t_callId));
    emit sigSendEvent(rpcResultEvent);
    m_pendingRpcHash.remove(t_callId);
}

void PriorityArbitrationSystem::arbitrationAdd(const QUuid &t_callId, const QVariantMap &t_parameters)
{
    ///@todo remove uid and use callid instead?
    QSet<QString> requiredParamKeys = {"uid", "priority"};
    QVariantMap retVal = t_parameters;//copy parameters and other data, the client could have attached additional data
    const QVariantMap parameters = t_parameters.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
    QStringList parameterNameList(parameters.keys());
    requiredParamKeys.subtract(QSet<QString>(parameterNameList.begin(), parameterNameList.end()));

    if(requiredParamKeys.isEmpty())
    {
        const int arbPriority = parameters.value("priority").toInt();
        if(arbPriority<0)
        {
            retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_EINVAL);
            retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, "Invalid parameter for priority, required is 0 <= priority <= INT32_MAX");
        }
        else if(m_priorityArbitrationMap.contains(arbPriority))
        {
            retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_UNAVAILABLE);
            retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, "The selected priority level is already reserved");
        }
        else
        {
            const QUuid peerIdentity = parameters.value("uid").toUuid();
            if(peerIdentity.isNull())
            {
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_EINVAL);
                retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, "Invalid parameter value for uid");
            }
            else
            {
                m_priorityArbitrationMap.insert(arbPriority, peerIdentity);
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_SUCCESS);
            }
        }
    }
    else
    {
        retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_EINVAL);
        retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Missing required parameters: [%1]").arg(requiredParamKeys.values().join(',')));
    }
    rpcFinished(t_callId, PriorityArbitrationSystem::s_arbitrationAddProcedureName, retVal);
}

void PriorityArbitrationSystem::arbitrationRemove(const QUuid &t_callId, const QVariantMap &t_parameters)
{
    QSet<QString> requiredParamKeys = {"uid"};
    QVariantMap retVal = t_parameters;//copy parameters and other data, the client could attach tracking
    const QVariantMap parameters = t_parameters.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
    QStringList parameterNameList(parameters.keys());
    requiredParamKeys.subtract(QSet<QString>(parameterNameList.begin(), parameterNameList.end()));

    const QUuid peerId = parameters.value("uid").toUuid();
    const int tmpKey = m_priorityArbitrationMap.key(peerId, -1);

    if(requiredParamKeys.isEmpty())
    {
        if(peerId.isNull() == false
                && tmpKey >= 0)
        {
            if(t_callId == peerId)
            {
                m_priorityArbitrationMap.remove(tmpKey);
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_SUCCESS);
            }
            else
            {
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_EINVAL);
                retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, "Caller id is different from uid parameter, removing arbitration for other clients is forbidden");
            }
        }
        else
        {
            retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_EINVAL);
            retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, "Invalid parameter value for uid, no client registered with this id");
        }
    }
    else
    {
        retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::PAS_EINVAL);
        retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Missing required parameters: [%1]").arg(requiredParamKeys.values().join(',')));
    }
    rpcFinished(t_callId, PriorityArbitrationSystem::s_arbitrationAddProcedureName, retVal);
}


//constexpr definition, see: https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
constexpr QLatin1String PriorityArbitrationSystem::s_entityName;
constexpr QLatin1String PriorityArbitrationSystem::s_entityNameComponentName;
constexpr QLatin1String PriorityArbitrationSystem::s_entityNameComponentDescription;
constexpr QLatin1String PriorityArbitrationSystem::s_clientPriorityListComponentName;
constexpr QLatin1String PriorityArbitrationSystem::s_clientPriorityListComponentDescription;
//rpc
constexpr QLatin1String PriorityArbitrationSystem::s_arbitrationAddProcedureName;
constexpr QLatin1String PriorityArbitrationSystem::s_arbitrationAddProcedureDescription;
constexpr QLatin1String PriorityArbitrationSystem::s_arbitrationRemoveProcedureName;
constexpr QLatin1String PriorityArbitrationSystem::s_arbitrationRemoveProcedureDescription;
