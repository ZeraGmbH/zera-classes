#include <scpi.h>

#include "scpidelegate.h"

namespace SCPIMODULE
{

cSCPIDelegate::cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface)
    :cSCPIObject(cmd, type)
{
    scpiInterface->genSCPICmd(cmdParent.split(":"), this);
}


bool cSCPIDelegate::executeSCPI(const QString &, QString &)
{
    return true;
}


}
