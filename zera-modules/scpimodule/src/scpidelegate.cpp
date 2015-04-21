#include <scpi.h>

#include "scpidelegate.h"

namespace SCPIMODULE
{

cSCPIDelegate::cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode)
    :cSCPIObject(cmd, type), m_nCmdCode(cmdCode)
{
    scpiInterface->genSCPICmd(cmdParent.split(":"), this);
}


bool cSCPIDelegate::executeSCPI(const QString &, QString &)
{
    return true;
}


bool cSCPIDelegate::executeSCPI(const QString &sInput)
{
    emit execute(m_nCmdCode,(QString&) sInput);
    return true;
}

}
