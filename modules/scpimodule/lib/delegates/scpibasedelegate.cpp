#include "scpibasedelegate.h"
#include <scpi.h>

namespace SCPIMODULE {

ScpiBaseDelegate::ScpiBaseDelegate() :
    ScpiObject("", 0)
{
}

ScpiBaseDelegate::ScpiBaseDelegate(QString cmdParent, QString cmd, quint8 type) :
    ScpiObject(cmd, type),
    m_sCmdParent(cmdParent)
{
}

void ScpiBaseDelegate::setCommand(cSCPI *scpiCmdInterface, ScpiBaseDelegatePtr delegate)
{
    scpiCmdInterface->insertScpiCmd(m_sCmdParent.split(":", Qt::SkipEmptyParts), delegate);
}

}
