#include <veinentity.h>

#include "scpistatus.h"
#include "signalconnectiondelegate.h"


namespace SCPIMODULE
{


cSignalConnectionDelegate::cSignalConnectionDelegate(cSCPIStatus *scpiStatus, quint8 bitnr)
    :m_pSCPIStatus(scpiStatus), m_nBitNr(bitnr)
{
}


void cSignalConnectionDelegate::setStatus(QVariant signal)
{
    if (signal != 0)
        m_pSCPIStatus->SetConditionBit(m_nBitNr, 1);
    else
        m_pSCPIStatus->SetConditionBit(m_nBitNr, 0);

}



}
