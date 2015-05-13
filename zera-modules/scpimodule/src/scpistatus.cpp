#include "scpistatus.h"

namespace SCPIMODULE
{


cSCPIStatus::cSCPIStatus(quint16 tothrow)
    :m_n2Throw(tothrow)
{
    // we have an all clear scpi conform status system
    m_nCondition = m_nPTransition = m_nNTransition = m_nEvent = m_nEnable = 0;
}


void cSCPIStatus::setCondition(quint16 condition)
{
    quint16 changedCondition;

    changedCondition = m_nCondition ^ condition;
    m_nCondition = condition;

    m_nEvent = (changedCondition & ((m_nPTransition & m_nCondition) | (m_nNTransition & (~m_nCondition))));
    if ( (m_nEvent & m_nEnable) > 0 )
        emit event(m_n2Throw);

}


}
