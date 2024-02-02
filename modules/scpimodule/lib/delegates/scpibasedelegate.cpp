#include "scpibasedelegate.h"
#include <scpi.h>

namespace SCPIMODULE {

ScpiBaseDelegate::ScpiBaseDelegate() :
    cSCPIObject("", 0)
{
}

ScpiBaseDelegate::ScpiBaseDelegate(QString cmdParent, QString cmd, quint8 type) :
    cSCPIObject(cmd, type),
    m_sCmdParent(cmdParent)
{
}

void ScpiBaseDelegate::setCommand(cSCPI *scpiCmdInterface)
{
    scpiCmdInterface->insertScpiCmd(m_sCmdParent.split(":", Qt::SkipEmptyParts), this);
}

}
