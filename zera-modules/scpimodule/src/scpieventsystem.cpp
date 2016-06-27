#include <QEvent>
#include <QHash>
#include <QMultiHash>

#include <scpi.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>

#include "scpieventsystem.h"
#include "scpimodule.h"
#include "scpiclient.h"
#include "scpiclientinfo.h"
#include "signalconnectiondelegate.h"
#include "scpimeasure.h"


namespace SCPIMODULE
{

cSCPIEventSystem::cSCPIEventSystem(cSCPIModule* module)
    :m_pModule(module), VeinEvent::EventSystem()
{
}


bool cSCPIEventSystem::processEvent(QEvent *t_event)
{
    bool retVal = false;

    if(t_event->type() == VeinEvent::CommandEvent::eventType())
    {
        VeinEvent::CommandEvent *cEvent = 0;
        cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        if(cEvent != 0)
        {
            switch(cEvent->eventSubtype())
            {
            case VeinEvent::CommandEvent::EventSubtype::NOTIFICATION:
                retVal = true;
                processCommandEvent(cEvent);
                break;

            }
        }
    }
    return retVal;
}


void cSCPIEventSystem::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
{
    // is it a command event for setting component data
    if (t_cEvent->eventData()->type() == VeinComponent::ComponentData::dataType())
    {
        // only notifications will be handled
        if (t_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION)
        {
            QString cName;
            int entityId;
            VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (t_cEvent->eventData());
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
            if (m_pModule->scpiClientInfoHash.contains(cName))
            {
                cSCPIClientInfo *clientinfo;
                clientinfo = m_pModule->scpiClientInfoHash.value(cName);
                if (clientinfo->entityId() == entityId)
                {
                    m_pModule->scpiClientInfoHash.remove(cName); // so sync is done
                    clientinfo->getClient()->receiveStatus(SCPI::ack);
                }

            }

            // then it looks for measurement values
            QList<cSCPIMeasure*> scpiMeasureList = m_pModule->scpiMeasureHash.values(cName);
            n = scpiMeasureList.count();

            if (n > 0)
            {
                for (int i = 0; i < n; i++)
                {
                    cSCPIMeasure *scpiMeasure;
                    scpiMeasure = scpiMeasureList.at(i);
                    if (scpiMeasure->entityID() == entityId)
                    {
                        scpiMeasure->initDone(cData->newValue()); // once we informed the object
                        m_pModule->scpiMeasureHash.remove(cName, scpiMeasure); // we remove it from the list
                    }
                }
            }

        }
    }

    else
        // or is it command event for error notification
        if (t_cEvent->eventData()->type() == VeinComponent::ErrorData::dataType() )
        {
            // only notifications will be handled
            if (t_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION)
            {
                QString cName;
                int entityId;
                VeinComponent::ErrorData* eData;
                eData = static_cast<VeinComponent::ErrorData*> (t_cEvent->eventData());
                VeinComponent::ComponentData* cData = new VeinComponent::ComponentData();
                cData->deserialize(eData->originalData());

                cName = cData->componentName();
                entityId = eData->entityId();

                // error notifications are sent for invalid parameters
                if (m_pModule->scpiClientInfoHash.contains(cName))
                {
                    cSCPIClientInfo *clientinfo;
                    clientinfo = m_pModule->scpiClientInfoHash.value(cName);
                    if (clientinfo->entityId() == entityId)
                    {
                        t_cEvent->accept(); // we caused the error event due to wrong parameter
                        m_pModule->scpiClientInfoHash.remove(cName); // command was handled
                        clientinfo->getClient()->receiveStatus(SCPI::errval);
                    }
                }
            }
        }
}

}
