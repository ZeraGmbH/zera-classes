#ifndef SCPIMODULECOMMONFUNCTIONS_H
#define SCPIMODULECOMMONFUNCTIONS_H

#include <QString>

class ScpiModuleCommonFunctions
{
public:
    static bool isQuery(const QString &scpi);

    static QString getScpiFullPath(const QString &cmdParent, const QString &cmd);
    static QString getMeasureScpiModuleName(const QString &scpiPath);
};

#endif // SCPIMODULECOMMONFUNCTIONS_H
