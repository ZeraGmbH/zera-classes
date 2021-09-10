#include <scpi.h>

#include "scpidelegate.h"

namespace SCPIMODULE
{

cSCPIDelegate::cSCPIDelegate(QString cmdParent, QString cmd, quint8 type)
    :cSCPIObject(cmd, type), m_sCmdParent(cmdParent)
{
}


void cSCPIDelegate::setCommand(cSCPI *scpiCmdInterface)
{
    scpiCmdInterface->genSCPICmd(m_sCmdParent.split(":"), this);
}


}
