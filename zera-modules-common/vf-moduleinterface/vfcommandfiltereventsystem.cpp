#include "vfcommandfiltereventsystem.h"

VfCommandFilterEventSystem::VfCommandFilterEventSystem(VeinEvent::CommandEvent::EventSubtype subtypeToFilter) :
    m_subtypeToFilter(subtypeToFilter)
{
}

bool VfCommandFilterEventSystem::processEvent(QEvent *event)
{
    bool retVal = false;
    if(event->type() == VeinEvent::CommandEvent::eventType()) {
        VeinEvent::CommandEvent *commandEvent = static_cast<VeinEvent::CommandEvent *>(event);
        if(commandEvent && commandEvent->eventSubtype() == m_subtypeToFilter) {
            retVal = true;
            processCommandEvent(commandEvent);
        }
    }
    return retVal;
}
