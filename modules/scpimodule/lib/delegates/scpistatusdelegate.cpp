#include "scpistatusdelegate.h"

namespace SCPIMODULE {

cSCPIStatusDelegate::cSCPIStatusDelegate(const QString &cmdParent, const QString &cmd, quint8 type, quint8 cmdCode, quint8 statindex) :
    ScpiBaseDelegate(cmdParent, cmd, type),
    m_nCmdCode(cmdCode), m_nStatusIndex(statindex)
{
}

void cSCPIStatusDelegate::executeSCPI(cSCPIClient *client, const QString &scpi)
{
    emit signalExecuteSCPI(client, m_nCmdCode, m_nStatusIndex, scpi);
}

}
