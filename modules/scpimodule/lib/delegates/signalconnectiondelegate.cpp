#include "signalconnectiondelegate.h"

namespace SCPIMODULE {

cSignalConnectionDelegate::cSignalConnectionDelegate(cSCPIStatus *scpiStatus, quint8 bitnr, int entityid, QString cname) :
    m_pSCPIStatus(scpiStatus),
    m_nBitNr(bitnr),
    m_nEntityId(entityid),
    m_sComponentName(cname)
{
}

int cSignalConnectionDelegate::EntityId()
{
   return m_nEntityId;
}

QString cSignalConnectionDelegate::ComponentName()
{
    return m_sComponentName;
}

void cSignalConnectionDelegate::setStatus(QVariant signal)
{
    if (signal != 0)
        m_pSCPIStatus->SetConditionBit(m_nBitNr, 1);
    else
        m_pSCPIStatus->SetConditionBit(m_nBitNr, 0);

}

}
