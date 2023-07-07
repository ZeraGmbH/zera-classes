#include "vfeventsystemcommandfilter.h"

VfEventSystemCommandFilter::VfEventSystemCommandFilter(VeinEvent::CommandEvent::EventSubtype subtypeToFilter) :
    m_subtypeToFilter(subtypeToFilter)
{
}

void VfEventSystemCommandFilter::processEvent(QEvent *event)
{
    if(event->type() == VeinEvent::CommandEvent::eventType()) {
        VeinEvent::CommandEvent *commandEvent = static_cast<VeinEvent::CommandEvent *>(event);
        if(commandEvent && commandEvent->eventSubtype() == m_subtypeToFilter) {
            processCommandEvent(commandEvent);
        }
    }
}
