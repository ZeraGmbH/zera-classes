#ifndef SCPIMODULECOMMONSTATICFUNCTIONS_H
#define SCPIMODULECOMMONSTATICFUNCTIONS_H

#include <QString>

class ScpiModuleCommonStaticFunctions
{
public:
    static bool isQuery(const QString &scpi);

    static QString getScpiFullPath(const QString &cmdParent, const QString &cmd);
    static QString getMeasureScpiModuleName(const QString &scpiPath);
};

#endif // SCPIMODULECOMMONSTATICFUNCTIONS_H
