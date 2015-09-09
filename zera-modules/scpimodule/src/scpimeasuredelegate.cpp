#include <scpi.h>
#include <scpicommand.h>

#include "scpiinterface.h"
#include "scpimeasuredelegate.h"
#include "scpimeasurecollector.h"
#include "scpimeasure.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIMeasureDelegate::cSCPIMeasureDelegate(QString cmdParent, QString cmd, quint8 type, quint8 measCode, cSCPIMeasure* scpimeasureobject)
    :cSCPIDelegate(cmdParent, cmd, type), m_nMeasCode(measCode)
{
    m_scpimeasureObjectList.append(scpimeasureobject);
}


bool cSCPIMeasureDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType;
    cSCPICommand cmd = sInput;

    scpiCmdType = getType();

    if ( (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) ||
         (cmd.isCommand(0) && ((scpiCmdType & SCPI::isCmd) > 0)) )
    {
        // allowed query or command
        cSCPIMeasureCollector* measureCollector = new cSCPIMeasureCollector(client, m_scpimeasureObjectList.count());

        for (int i = 0; i < m_scpimeasureObjectList.count(); i++)
        {
            cSCPIMeasure* measure = m_scpimeasureObjectList.at(i);
            connect(measure, SIGNAL(cmdAnswer(QString)), measureCollector, SLOT(receiveAnswer(QString)));
            connect(measure, SIGNAL(cmdStatus(quint8)), measureCollector, SLOT(receiveStatus(quint8)));
            measure->execute(m_nMeasCode);
        }
    }
    else
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalStatus(quint8)), client, SLOT(receiveStatus(quint8)));
        emit signalStatus(SCPI::nak);
        disconnect(myConn);
        // client->receiveStatus(SCPI::nak);
    }

    return true;
}


void cSCPIMeasureDelegate::addscpimeasureObject(cSCPIMeasure *measureobject)
{
    m_scpimeasureObjectList.append(measureobject);
}

}
