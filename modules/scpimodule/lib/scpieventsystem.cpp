#include "scpieventsystem.h"
#include "scpiclient.h"
#include "scpiveintransactioninfo.h"
#include "signalconnectiondelegate.h"
#include "scpiserver.h"
#include "scpigroupmeasureandfriends.h"
#include <zscpi_response_definitions.h>
#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>

namespace SCPIMODULE
{

SCPIEventSystem::SCPIEventSystem(cSCPIModule* module) :
    VfEventSystemCommandFilter(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION),
    m_pModule(module)
{
}

void SCPIEventSystem::processCommandEvent(VeinEvent::CommandEvent *commandEvent)
{
    if(commandEvent->eventData()->type() == VeinComponent::ComponentData::dataType())
        handleComponentData(commandEvent);
    else if(commandEvent->eventData()->type() == VeinComponent::ErrorData::dataType())
        handleErrorData(commandEvent);
}

void SCPIEventSystem::handleComponentData(VeinEvent::CommandEvent *commandEvent)
{
    if(commandEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION) {
        const VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*>(commandEvent->eventData());
        const int entityId = cData->entityId();
        const QString componentName = cData->componentName();

        // handle configured signal connections
        for(int i = 0; i < m_pModule->sConnectDelegateList.count(); i++) {
            cSignalConnectionDelegate* delegate = m_pModule->sConnectDelegateList.at(i);
            if(delegate->EntityId() == entityId && delegate->ComponentName() == componentName)
                delegate->setStatus(cData->newValue());
        }

        // then it looks for parameter values
        if(m_pModule->scpiParameterCmdInfoHash.contains(componentName)) {
            const QList<SCPIVeinTransactionInfoPtr> transactionInfoList = m_pModule->scpiParameterCmdInfoHash.values(componentName);
            for(int i = 0; i < transactionInfoList.count(); i++) {
                SCPIVeinTransactionInfoPtr transactionInfo = transactionInfoList.at(i);
                QUuid clientId = commandEvent->peerId();
                // test if server notification or client in transaction matches
                if(clientId.isNull() || clientId == transactionInfo->getClient()->getClientId()) {
                    if(transactionInfo->entityId() == entityId) {
                        m_pModule->scpiParameterCmdInfoHash.remove(componentName, transactionInfo);
                        QMetaObject::Connection myConn = connect(this, &SCPIEventSystem::sigClientInfoSignal,
                                                                 transactionInfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
                        emit sigClientInfoSignal(componentName);
                        disconnect(myConn);
                        if(transactionInfo->parCmdType() == parcmd) {
                            cSCPIClient* client = transactionInfo->getClient();
                            client->handleCmdFinishStatusOnly(ZSCPI::ack, transactionInfo->getScpiTransactionId());
                        }
                        else {
                            QString answer = static_cast<VeinComponent::ComponentData*>(commandEvent->eventData())->newValue().toString();
                            transactionInfo->getClient()->handleCmdFinish(answer, transactionInfo->getScpiTransactionId());
                        }
                        break;
                    }
                }
            }
        }

        // then it looks for measurement values
        const QList<ScpiMeasurePerVeinComponentSequencer*> scpiMeasureList = m_pModule->m_scpiMeasureHash->values(componentName);
        for(int i = 0; i < scpiMeasureList.count(); i++) {
            ScpiMeasurePerVeinComponentSequencer *scpiMeasure = scpiMeasureList.at(i);
            if(scpiMeasure->entityID() == entityId)
                scpiMeasure->receiveMeasureValue(cData->newValue());
        }

        // then it looks for changes on module interface components
        if( componentName == QString("INF_ModuleInterface"))
            m_pModule->getSCPIServer()->getModuleInterface()->actualizeInterface(cData->newValue());

    }
}

void SCPIEventSystem::handleErrorData(VeinEvent::CommandEvent *commandEvent)
{
    if(commandEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION) {
        const VeinComponent::ErrorData* eData = static_cast<VeinComponent::ErrorData*>(commandEvent->eventData());
        VeinComponent::ComponentData* cData = new VeinComponent::ComponentData();
        cData->deserialize(eData->originalData());
        const QString errorComponentName = cData->componentName();
        const int errorEntityId = eData->entityId();

        // error notifications are sent for invalid parameters
        if(m_pModule->scpiParameterCmdInfoHash.contains(errorComponentName)) {
            QList<SCPIVeinTransactionInfoPtr> transactionInfoList = m_pModule->scpiParameterCmdInfoHash.values();
            for (int i = 0; i < transactionInfoList.count(); i++) {
                SCPIVeinTransactionInfoPtr transactionInfo = transactionInfoList.at(i);
                if(transactionInfo->entityId() == errorEntityId) {
                    commandEvent->accept();  // we caused the error event due to wrong parameter
                    m_pModule->scpiParameterCmdInfoHash.remove(errorComponentName, transactionInfo);
                    QMetaObject::Connection myConn = connect(this, &SCPIEventSystem::sigClientInfoSignal,
                                                             transactionInfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
                    emit sigClientInfoSignal(errorComponentName);
                    disconnect(myConn);
                    cSCPIClient* client = transactionInfo->getClient();
                    client->handleCmdFinishStatusOnly(ZSCPI::errval, transactionInfo->getScpiTransactionId());
                    break;
                }
            }
        }
    }
}

}
