#include <QCoreApplication>
#include <QEvent>
#include <QHash>
#include <QMultiHash>

#include <scpi.h>
#include <ve_commandevent.h>
#include <ve_storagesystem.h>
#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>

#include "scpieventsystem.h"
#include "scpimodule.h"
#include "scpiclient.h"
#include "scpiclientinfo.h"
#include "signalconnectiondelegate.h"
#include "scpimeasure.h"
#include "scpiserver.h"
#include "moduleinterface.h"


namespace SCPIMODULE
{

cSCPIEventSystem::cSCPIEventSystem(cSCPIModule* module) :
      VfEventSystemCommandFilter(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION),
      m_pModule(module)
{
}

void cSCPIEventSystem::processCommandEvent(VeinEvent::CommandEvent *commandEvent)
{
    // is it a command event for setting component data
    if (commandEvent->eventData()->type() == VeinComponent::ComponentData::dataType())
    {
        // only notifications will be handled
        if (commandEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION)
        {
            QString cName;
            int entityId;
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (commandEvent->eventData());
            cName = cData->componentName();
            entityId = cData->entityId();

            // first the handler handles existing signal connections
            int n = m_pModule->sConnectDelegateList.count();

            if (n > 0)
            {
                for (int i = 0; i < n; i++)
                {
                    cSignalConnectionDelegate* sdelegate;
                    sdelegate = m_pModule->sConnectDelegateList.at(i);
                    if (sdelegate->EntityId() == entityId)
                    {
                        sdelegate->setStatus(cData->newValue());
                    }
                }
            }

            // then it looks for parameter values
            if (m_pModule->scpiParameterCmdInfoHash.contains(cName))
            {
                QList<SCPIClientInfoPtr> clientinfolist = m_pModule->scpiParameterCmdInfoHash.values(cName);
                for (int i = 0; i < clientinfolist.count(); i++)
                {
                    SCPIClientInfoPtr clientinfo = clientinfolist.at(i);

                    QUuid clientId;
                    clientId = commandEvent->peerId();
                    if (clientId.isNull() || clientId == clientinfo->getClient()->getClientId()) // test if this client sent command for this parameter
                    {
                        if (clientinfo->entityId() == entityId)
                        {
                            m_pModule->scpiParameterCmdInfoHash.remove(cName, clientinfo);
                            QMetaObject::Connection myConn = connect(this, &cSCPIEventSystem::clientinfoSignal, clientinfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
                            emit clientinfoSignal(cName);
                            disconnect(myConn);
                            if (clientinfo->parCmdType() == SCPIMODULE::parcmd)
                            {
                                myConn = connect(this, &cSCPIEventSystem::status, clientinfo->getClient(), &cSCPIClient::receiveStatus);
                                emit status(SCPI::ack);
                                disconnect(myConn);
                            }
                            else
                            {
                                QString answer = static_cast<VeinComponent::ComponentData*> (commandEvent->eventData())->newValue().toString();
                                //cSCPIClient* client = clientinfo->getClient();
                                clientinfo->getClient()->receiveAnswer(answer);
                            }
                            break;
                        }
                    }
                }
            }

            // then it looks for measurement values
            QList<cSCPIMeasure*> scpiMeasureList = m_pModule->scpiMeasureHash.values(cName);
            n = scpiMeasureList.count();

            if (n > 0)
            {
                for (int i = 0; i < n; i++)
                {
                    cSCPIMeasure *scpiMeasure = scpiMeasureList.at(i);
                    if (scpiMeasure->entityID() == entityId)
                        scpiMeasure->receiveMeasureValue(cData->newValue());
                }
            }

            // then it looks for changes on module interface components
            {
                if ( cName == QString("INF_ModuleInterface"))
                    m_pModule->getSCPIServer()->getModuleInterface()->actualizeInterface(cData->newValue());
            }

        }
    }

    else
        // or is it command event for error notification
        if (commandEvent->eventData()->type() == VeinComponent::ErrorData::dataType() )
        {
            // only notifications will be handled
            if (commandEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION)
            {
                QString cName;
                int entityId;
                VeinComponent::ErrorData* eData;
                eData = static_cast<VeinComponent::ErrorData*> (commandEvent->eventData());
                VeinComponent::ComponentData* cData = new VeinComponent::ComponentData();
                cData->deserialize(eData->originalData());

                cName = cData->componentName();
                entityId = eData->entityId();

                // error notifications are sent for invalid parameters
                if (m_pModule->scpiParameterCmdInfoHash.contains(cName))
                {
                    QList<SCPIClientInfoPtr> clientinfolist = m_pModule->scpiParameterCmdInfoHash.values();
                    for (int i = 0; i < clientinfolist.count(); i++)
                    {
                        SCPIClientInfoPtr clientinfo = clientinfolist.at(i);
                        if (clientinfo->entityId() == entityId)
                        {
                            commandEvent->accept();  // we caused the error event due to wrong parameter
                            m_pModule->scpiParameterCmdInfoHash.remove(cName, clientinfo);
                            QMetaObject::Connection myConn = connect(this, &cSCPIEventSystem::clientinfoSignal, clientinfo->getClient(), &cSCPIClient::removeSCPIClientInfo, Qt::QueuedConnection);
                            emit clientinfoSignal(cName);
                            disconnect(myConn);
                            myConn = connect(this, &cSCPIEventSystem::status, clientinfo->getClient(), &cSCPIClient::receiveStatus);
                            emit status(SCPI::errval);
                            disconnect(myConn);
                            break;
                        }
                    }
                }
            }
        }
}

}
