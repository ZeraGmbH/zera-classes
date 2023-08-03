#include "scpimeasurecollector.h"
#include "scpiclient.h"

namespace SCPIMODULE
{


cSCPIMeasureCollector::cSCPIMeasureCollector(cSCPIClient *client, quint32 nr)
    :m_pClient(client), m_nmeasureObjects(nr), m_nmeasureObjectsInit(nr)
{
    m_nStatus = 0;
}


void cSCPIMeasureCollector::receiveStatus(quint8 stat)
{
    m_nStatus |= stat;
    m_nmeasureObjects--;
    if (m_nmeasureObjects == 0)
    {
        m_pClient->receiveStatus(m_nStatus);
        deleteLater();
    }

}


void cSCPIMeasureCollector::receiveAnswer(QString s)
{
    m_sAnswer += QString("%1;").arg(s);
    m_nmeasureObjects--;
    if (m_nmeasureObjects == 0)
    {
        m_pClient->receiveAnswer(m_sAnswer);
        deleteLater();
    }
}


}
