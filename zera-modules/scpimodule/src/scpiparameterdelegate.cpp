#include <scpi.h>
#include <scpicommand.h>
#include <veinentity.h>

#include "scpiparameterdelegate.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIParameterDelegate::cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, VeinPeer* peer, VeinEntity* entity)
    :cSCPIDelegate(cmdParent, cmd, type), m_pPeer(peer), m_pEntity(entity)
{
}


bool cSCPIParameterDelegate::executeSCPI(cSCPIClient *client, const QString &sInput)
{
    quint8 scpiCmdType;

    scpiCmdType = getType();
    cSCPICommand cmd = sInput;

    QMetaObject::Connection myConn = connect(this, SIGNAL(signalAnswer(QString)), client, SLOT(receiveAnswer(QString)));

    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) // test if we got an allowed query
    {
        QString answer = m_pEntity->getValue().toString();
        emit signalAnswer(answer);
        //client->receiveAnswer(answer);
    }
    else

        if (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) // test if we got an allowed cmd + 1 parameter
        {
            bool validation;
            if (validation = true) // todo here
            {
                QVariant var = m_pEntity->getValue();
                QVariant newVar = cmd.getParam(0);
                newVar.convert(var.type());
                m_pEntity->setValue(newVar, m_pPeer);
                emit signalStatus(SCPI::ack);
                //client->receiveStatus(SCPI::ack);
            }
            else
                emit signalStatus(SCPI::errval);
                //client->receiveStatus(SCPI::errval);

        }

        else
            emit signalStatus(SCPI::nak);
            //client->receiveStatus(SCPI::nak);

    disconnect(myConn);

    return true;
}

}
