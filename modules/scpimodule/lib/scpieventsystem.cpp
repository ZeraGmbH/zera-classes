#include "scpieventsystem.h"
#include "scpiclient.h"
#include "scpiclientinfo.h"
#include "signalconnectiondelegate.h"
#include "scpiserver.h"
#include "moduleinterface.h"
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
            const QList<SCPIClientInfoPtr> clientInfoList = m_pModule->scpiParameterCmdInfoHash.values(componentName);
            for(int i = 0; i < clientInfoList.count(); i++) {
                SCPIClientInfoPtr clientinfo = clientInfoList.at(i);
                QUuid clientId = commandEvent->peerId();
                // test if this client sent command for this parameter
                if(clientId.isNull() || clientId == clientinfo->getClient()->getClientId()) {
                    if(clientinfo->entityId() == entityId) {
                        m_pModule->scpiParameterCmdInfoHash.remove(componentName, clientinfo);
                        QMetaObject::Connection myConn = connect(this, &SCPIEventSystem::sigClientInfoSignal,
                                                                 clientinfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
                        emit sigClientInfoSignal(componentName);
                        disconnect(myConn);
                        if(clientinfo->parCmdType() == parcmd) {
                            cSCPIClient* client = clientinfo->getClient();
                            client->receiveStatus(ZSCPI::ack);
                        }
                        else {
                            QString answer = static_cast<VeinComponent::ComponentData*>(commandEvent->eventData())->newValue().toString();
                            clientinfo->getClient()->receiveAnswer(answer);
                        }
                        break;
                    }
                }
            }
        }

        // then it looks for measurement values
        const QList<cSCPIMeasure*> scpiMeasureList = m_pModule->m_scpiMeasureHash->values(componentName);
        for(int i = 0; i < scpiMeasureList.count(); i++) {
            cSCPIMeasure *scpiMeasure = scpiMeasureList.at(i);
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
            QList<SCPIClientInfoPtr> clientinfolist = m_pModule->scpiParameterCmdInfoHash.values();
            for (int i = 0; i < clientinfolist.count(); i++) {
                SCPIClientInfoPtr clientinfo = clientinfolist.at(i);
                if(clientinfo->entityId() == errorEntityId) {
                    commandEvent->accept();  // we caused the error event due to wrong parameter
                    m_pModule->scpiParameterCmdInfoHash.remove(errorComponentName, clientinfo);
                    QMetaObject::Connection myConn = connect(this, &SCPIEventSystem::sigClientInfoSignal,
                                                             clientinfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
                    emit sigClientInfoSignal(errorComponentName);
                    disconnect(myConn);
                    cSCPIClient* client = clientinfo->getClient();
                    client->receiveStatus(ZSCPI::errval);
                    break;
                }
            }
        }
    }
}

}
