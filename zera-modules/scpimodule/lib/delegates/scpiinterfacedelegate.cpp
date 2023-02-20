#include "scpiinterfacedelegate.h"

namespace SCPIMODULE {

cSCPIInterfaceDelegate::cSCPIInterfaceDelegate(QString cmdParent, QString cmd, quint8 type,  quint16 cmdCode) :
    ScpiBaseDelegate(cmdParent, cmd, type),
    m_nCmdCode(cmdCode)
{
}

bool cSCPIInterfaceDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    emit signalExecuteSCPI(client, m_nCmdCode, sInput);
    return true;
}

}
