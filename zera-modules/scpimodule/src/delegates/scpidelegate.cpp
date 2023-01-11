#include <scpi.h>

#include "scpidelegate.h"

namespace SCPIMODULE
{

cSCPIDelegate::cSCPIDelegate() :
    cSCPIObject("", 0)
{
}

cSCPIDelegate::cSCPIDelegate(QString cmdParent, QString cmd, quint8 type)
    :cSCPIObject(cmd, type), m_sCmdParent(cmdParent)
{
}


void cSCPIDelegate::setCommand(cSCPI *scpiCmdInterface)
{
    scpiCmdInterface->insertScpiCmd(m_sCmdParent.split(":"), this);
}


}
