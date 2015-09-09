#include "scpistatusdelegate.h"

namespace SCPIMODULE
{

cSCPIStatusDelegate::cSCPIStatusDelegate(QString cmdParent, QString cmd, quint8 type, quint8 cmdCode, quint8 statindex)
    :cSCPIDelegate(cmdParent, cmd, type), m_nCmdCode(cmdCode), m_nStatusIndex(statindex)
{
}


bool cSCPIStatusDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    emit executeSCPI(client, m_nCmdCode, m_nStatusIndex, sInput);
    return true;
}


}
