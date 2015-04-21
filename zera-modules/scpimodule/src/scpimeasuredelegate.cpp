#include "scpimeasuredelegate.h"

namespace SCPIMODULE
{

cSCPIMeasureDelegate::cSCPIMeasureDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode, cSCPIMeasure* scpimeasureobject)
    :cSCPIDelegate(cmdParent, cmd, type, scpiInterface, cmdCode)
{
    m_scpimeasureObjectList.append(scpimeasureobject);
}


void cSCPIMeasureDelegate::addscpimeasureObject(cSCPIMeasure *measureobject)
{
    m_scpimeasureObjectList.append(measureobject);
}


QList<cSCPIMeasure*>* cSCPIMeasureDelegate::getscpimeasureObjectList()
{
    return &m_scpimeasureObjectList;
}

}
