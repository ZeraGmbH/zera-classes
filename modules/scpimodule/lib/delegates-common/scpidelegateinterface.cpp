#include "scpidelegateinterface.h"

namespace SCPIMODULE {

ScpiDelegateInterface::ScpiDelegateInterface(const QString &cmdParent, const QString &cmd,
                                               quint8 scpiCmdQueryFlags,
                                               quint16 cmdCode,
                                               const QString &cmdDescription) :
    ScpiDelegateTemplate(cmdParent, cmd, scpiCmdQueryFlags),
    m_nCmdCode(cmdCode)
{
    if(!cmdDescription.isEmpty())
        setXmlAttribute("Description", cmdDescription);
}

void ScpiDelegateInterface::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    emit signalExecuteSCPI(client, m_nCmdCode, scpi, scpiTransactionId);
}

}
