#include "scpidelegateinterface.h"

namespace SCPIMODULE {

ScpiDelegateInterface::ScpiDelegateInterface(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiCmdQueryFlags),
    m_nCmdCode(params.cmdCode)
{
    if(!params.cmdDescription.isEmpty())
        setXmlAttribute("Description", params.cmdDescription);
}

void ScpiDelegateInterface::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    emit signalExecuteSCPI(client, m_nCmdCode, scpi, scpiTransactionId);
}

}
