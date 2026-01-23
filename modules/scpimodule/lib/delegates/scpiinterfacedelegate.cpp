#include "scpiinterfacedelegate.h"

namespace SCPIMODULE {

cSCPIInterfaceDelegate::cSCPIInterfaceDelegate(const QString &cmdParent, const QString &cmd, quint8 type,  quint16 cmdCode, const QString &cmdDescription) :
    ScpiBaseDelegate(cmdParent, cmd, type),
    m_nCmdCode(cmdCode)
{
    if(!cmdDescription.isEmpty())
        setXmlAttribute("Description", cmdDescription);
}

void cSCPIInterfaceDelegate::executeSCPI(cSCPIClient *client, const QString &scpi)
{
    emit signalExecuteSCPI(client, m_nCmdCode, scpi);
}

}
