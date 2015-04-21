#include "scpinonmeasuredelegate.h"

namespace SCPIMODULE
{

cSCPInonMeasureDelegate::cSCPInonMeasureDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode, VeinPeer* peer, VeinEntity* entity)
    :cSCPIDelegate(cmdParent, cmd, type, scpiInterface, cmdCode), m_pPeer(peer), m_pEntity(entity)
{
}


VeinPeer* cSCPInonMeasureDelegate::getPeer()
{
    return m_pPeer;
}


VeinEntity* cSCPInonMeasureDelegate::getEntity()
{
    return m_pEntity;
}

}
