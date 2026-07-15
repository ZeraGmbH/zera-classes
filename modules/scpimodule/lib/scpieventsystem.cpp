#include "scpieventsystem.h"
#include "scpiclient.h"
#include "scpiveintransactioninfo.h"
#include "signalconnectiondelegate.h"
#include "scpiserver.h"
#include "scpimodelmeasureandfriends.h"
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
        m_pModule->updateSignalConnection(entityId, componentName, cData->newValue());

        // then it looks for parameter values
        const QList<SCPIVeinTransactionInfoPtr> transactionInfoList = m_pModule->getScpiVeinParamRpcTransactions(componentName);
        for (int i = 0; i < transactionInfoList.count(); i++) {
            SCPIVeinTransactionInfoPtr transactionInfo = transactionInfoList.at(i);
            QUuid clientId = commandEvent->peerId();
            // test if server notification or client in transaction matches
            if (clientId.isNull() || clientId == transactionInfo->getClient()->getClientId()) {
                if (transactionInfo->entityId() == entityId) {
                    m_pModule->removeScpiVeinParamRpcTransaction(componentName, transactionInfo);
                    QMetaObject::Connection myConn = connect(this, &SCPIEventSystem::sigClientInfoSignal,
                                                             transactionInfo->getClient(), &cSCPIClient::removeVeinParamRpcTransactionInfo, Qt::QueuedConnection);
                    emit sigClientInfoSignal(componentName);
                    disconnect(myConn);
                    if (transactionInfo->parCmdType() == parcmd) {
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

        // then it looks for measurement values
        m_pModule->updatePendingMeasureSequences(entityId, componentName, cData->newValue());

        // then it looks for changes on module interface components
        if( componentName == QString("INF_ModuleInterface"))
            m_pModule->getSCPIServer()->getScpiModelCatalogs()->actualizeCatalogs(cData->newValue());

    }
}

void SCPIEventSystem::handleErrorData(VeinEvent::CommandEvent *commandEvent)
{
    if (commandEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION) {
        const VeinComponent::ErrorData* eData = static_cast<VeinComponent::ErrorData*>(commandEvent->eventData());
        VeinComponent::ComponentData* cData = new VeinComponent::ComponentData();
        cData->deserialize(eData->originalData());
        const QString errorComponentName = cData->componentName();
        const int errorEntityId = eData->entityId();

        // error notifications are sent for invalid parameters
        const QList<SCPIVeinTransactionInfoPtr> transactionInfoList = m_pModule->getAllScpiVeinParamRpcTransactions();
        for (int i = 0; i < transactionInfoList.count(); i++) {
            SCPIVeinTransactionInfoPtr transactionInfo = transactionInfoList.at(i);
            if (transactionInfo->entityId() == errorEntityId) {
                commandEvent->accept();  // we caused the error event due to wrong parameter
                m_pModule->removeScpiVeinParamRpcTransaction(errorComponentName, transactionInfo);
                QMetaObject::Connection myConn = connect(this, &SCPIEventSystem::sigClientInfoSignal,
                                                         transactionInfo->getClient(), &cSCPIClient::removeVeinParamRpcTransactionInfo, Qt::QueuedConnection);
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
