#include "scpimeasurecollector.h"

namespace SCPIMODULE
{


cSCPIMeasureCollector::cSCPIMeasureCollector(quint32 nr)
    :m_nmeasureObjects(nr)
{
    m_nStatus = 0;
}


void cSCPIMeasureCollector::receiveStatus(quint8 stat)
{
    m_nStatus |= stat;
    m_nmeasureObjects--;
    if (m_nmeasureObjects == 0)
    {
        emit cmdStatus(m_nStatus);
        deleteLater();
    }

}


void cSCPIMeasureCollector::receiveAnswer(QString s)
{
    m_sAnswer += QString("%1;").arg(s);
    if (m_nmeasureObjects == 0)
    {
        emit cmdAnswer(m_sAnswer);
        deleteLater();
    }
}





}
