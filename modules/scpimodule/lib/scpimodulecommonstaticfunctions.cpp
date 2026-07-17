#include "scpimodulecommonstaticfunctions.h"
#include <scpicommand.h>

bool ScpiModuleCommonStaticFunctions::isQuery(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    return cmd.isQuery() || cmd.isQuery(1); // we allow queries without or with 1 parameter
}

QString ScpiModuleCommonStaticFunctions::getScpiFullPath(const QString &cmdParent, const QString &cmd)
{
    const QStringList cmdParentList = cmdParent.split(":", Qt::SkipEmptyParts);
    const QStringList cmdList = cmd.split(":", Qt::SkipEmptyParts);
    const QStringList fullPathList = cmdParentList + cmdList;
    return fullPathList.join(":");
}

QString ScpiModuleCommonStaticFunctions::getMeasureScpiModuleName(const QString &scpiPath)
{
    const QStringList scpiPathList = scpiPath.split(":", Qt::SkipEmptyParts);
    if (scpiPathList.size() > 1)
        return scpiPathList[1];
    return QString();
}
