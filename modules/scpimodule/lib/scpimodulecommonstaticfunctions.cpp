#include "scpimodulecommonstaticfunctions.h"
#include <scpicommand.h>

bool ScpiModuleCommonStaticFunctions::isQuery(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    return cmd.isQuery() || cmd.isQuery(1); // we allow queries without or with 1 parameter
}
