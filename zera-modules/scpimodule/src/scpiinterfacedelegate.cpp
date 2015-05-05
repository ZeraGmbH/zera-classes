#include "scpiinterfacedelegate.h"
#include "scpiclient.h"


namespace SCPIMODULE
{


cSCPIInterfaceDelegate::cSCPIInterfaceDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode)
    :cSCPIDelegate(cmdParent, cmd, type, scpiInterface), m_nCmdCode(cmdCode)
{

}


bool cSCPIInterfaceDelegate::executeSCPI(const QString &, QString &)
{
    return true;
}


bool cSCPIInterfaceDelegate::executeSCPI(cSCPIClient *client, const QString &sInput)
{
    emit executeSCPI(client, m_nCmdCode, (QString&) sInput);
    return true;
}

}
