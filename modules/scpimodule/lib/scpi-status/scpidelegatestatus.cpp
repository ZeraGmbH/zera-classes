#include "scpidelegatestatus.h"

namespace SCPIMODULE {

ScpiDelegateStatus::ScpiDelegateStatus(const QString &cmdParent, const QString &cmd,
                                       quint8 scpiCmdQueryFlags,
                                       SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                                       SCPIStatusDefinitions::ScpiStatusSystems statusSystemIdx) :
    ScpiDelegateTemplate(cmdParent, cmd, scpiCmdQueryFlags),
    m_cmdCode(cmdCode),
    m_statusSystemIdx(statusSystemIdx)
{
}

void ScpiDelegateStatus::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    emit signalExecuteSCPI(client, m_cmdCode, m_statusSystemIdx, scpi, scpiTransactionId);
}

}
