#include "scpimeasurecollector.h"
#include "scpiclient.h"

namespace SCPIMODULE
{


cSCPIMeasureCollector::cSCPIMeasureCollector(cSCPIClient *client, quint32 nr)
    :m_pClient(client), m_nmeasureObjects(nr)
{
    m_nStatus = 0;
}


void cSCPIMeasureCollector::receiveStatus(quint8 stat)
{
    m_nStatus |= stat;
    m_nmeasureObjects--;
    if (m_nmeasureObjects == 0)
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalStatus(quint8)), m_pClient, SLOT(receiveStatus(quint8)));
        emit signalStatus(m_nStatus);
        disconnect(myConn);
        //m_pClient->receiveStatus(m_nStatus);
        deleteLater();
    }

}


void cSCPIMeasureCollector::receiveAnswer(QString s)
{
    m_sAnswer += QString("%1;").arg(s);
    m_nmeasureObjects--;
    if (m_nmeasureObjects == 0)
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalAnswer(QString)), m_pClient, SLOT(receiveAnswer(QString)));
        emit signalAnswer(m_sAnswer);
        disconnect(myConn);
        //m_pClient->receiveAnswer(m_sAnswer);
        deleteLater();
    }
}


}
