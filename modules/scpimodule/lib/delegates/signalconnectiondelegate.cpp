#include "signalconnectiondelegate.h"

namespace SCPIMODULE {

cSignalConnectionDelegate::cSignalConnectionDelegate(cSCPIStatus *scpiStatus, quint8 bitnr, int entityid, const QString &componentName) :
    m_pSCPIStatus(scpiStatus),
    m_nBitNr(bitnr),
    m_nEntityId(entityid),
    m_sComponentName(componentName)
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

void cSignalConnectionDelegate::setStatus(const QVariant &signal)
{
    if (signal != 0)
        m_pSCPIStatus->SetConditionBit(m_nBitNr, 1);
    else
        m_pSCPIStatus->SetConditionBit(m_nBitNr, 0);

}

}
