#include <ve_commandevent.h>
#include <vcmp_componentdata.h>

#include "basemoduleeventsystem.h"
#include "veinmodulecomponentinput.h"

bool cBaseModuleEventSystem::processEvent(QEvent *t_event)
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
              default:
                break;
            }
        }
    }
    return retVal;
}


void cBaseModuleEventSystem::setInputList(QList<cVeinModuleComponentInput *> &inputlist)
{
    m_InputList = inputlist;
}


void cBaseModuleEventSystem::processCommandEvent(VeinEvent::CommandEvent *t_cEvent)
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

            // now let's look if we need this input
            for (int i = 0; i < m_InputList.count(); i++)
            {
                cVeinModuleComponentInput *vmci = m_InputList.at(i);
                if ( (vmci->m_sName == cName) && (vmci->m_nEntityId == entityId) )
                {
                    vmci->setValue(cData->newValue());
                    emit vmci->sigValueChanged(cData->newValue());
                    // break; // we don't break here -> so a module can use an input several times
                }
            }
        }
    }
}


