#include <scpi.h>
#include <scpicommand.h>
#include <veinentity.h>

#include "scpiparameterdelegate.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIParameterDelegate::cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, VeinPeer* peer, VeinEntity* entity)
    :cSCPIDelegate(cmdParent, cmd, type, scpiInterface), m_pPeer(peer), m_pEntity(entity)
{
}


bool cSCPIParameterDelegate::executeSCPI(cSCPIClient *client, const QString &sInput)
{
    quint8 scpiCmdType;

    scpiCmdType = getType();
    cSCPICommand cmd = sInput;

    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) // test if we got an allowed query
    {
        QString answer = m_pEntity->getValue().toString();
        client->receiveAnswer(answer);
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
                client->receiveStatus(SCPI::ack);
            }
            else
                client->receiveStatus(SCPI::errval);

        }

        else
            client->receiveStatus(SCPI::nak);

    return true;
}

}
