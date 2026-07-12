#include "scpidelegatestatus.h"

namespace SCPIMODULE {

ScpiDelegateStatus::ScpiDelegateStatus(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiCmdQueryFlags),
    m_cmdCode(params.cmdCode),
    m_statusSystemIdx(params.statusSystemIdx)
{
}

void ScpiDelegateStatus::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    emit signalExecuteSCPI(client, m_cmdCode, m_statusSystemIdx, scpi, scpiTransactionId);
}

}
