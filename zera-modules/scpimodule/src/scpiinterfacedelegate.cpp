#include "scpiinterfacedelegate.h"
#include "scpiclient.h"


namespace SCPIMODULE
{


cSCPIInterfaceDelegate::cSCPIInterfaceDelegate(QString cmdParent, QString cmd, quint8 type,  quint16 cmdCode)
    :cSCPIDelegate(cmdParent, cmd, type), m_nCmdCode(cmdCode)
{

}


bool cSCPIInterfaceDelegate::executeSCPI(cSCPIClient *client, const QString &sInput)
{
    emit executeSCPI(client, m_nCmdCode, sInput);
    return true;
}

}
