#include "scpiinterfacedelegate.h"

namespace SCPIMODULE {

cSCPIInterfaceDelegate::cSCPIInterfaceDelegate(QString cmdParent, QString cmd, quint8 type,  quint16 cmdCode, QString cmdDescription) :
    ScpiBaseDelegate(cmdParent, cmd, type),
    m_nCmdCode(cmdCode)
{
    if(!cmdDescription.isEmpty())
        setXmlAttribute("Description", cmdDescription);
}

bool cSCPIInterfaceDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    emit signalExecuteSCPI(client, m_nCmdCode, sInput);
    return true;
}

}
