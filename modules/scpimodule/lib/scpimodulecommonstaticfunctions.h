#ifndef SCPIMODULECOMMONSTATICFUNCTIONS_H
#define SCPIMODULECOMMONSTATICFUNCTIONS_H

#include <QString>

class ScpiModuleCommonStaticFunctions
{
public:
    static bool isQuery(const QString &scpi);
};

#endif // SCPIMODULECOMMONSTATICFUNCTIONS_H
