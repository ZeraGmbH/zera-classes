#include "sourcestatecontrollerinternal.h"


void SourceStateControllerInternal::onSwitchTransactionStarted()
{
    setState(SourceStates::SWITCH_BUSY);
}

void SourceStateControllerInternal::setState(SourceStates state)
{
    if(m_currState != state) {
        m_currState = state;
        emit sigStateChanged(state);
    }
}
