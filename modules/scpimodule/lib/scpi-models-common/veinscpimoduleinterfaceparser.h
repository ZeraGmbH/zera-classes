#ifndef VEINSCPIMODULEINTERFACEPARSER_H
#define VEINSCPIMODULEINTERFACEPARSER_H

#include "scpicmdinfo.h"
#include <vs_abstractdatabase.h>
#include <QHash>

namespace SCPIMODULE
{

class VeinScpiModuleInterfaceParser
{
public:
    bool parseVeinStorage(const VeinStorage::AbstractDatabase* storageDb);

    typedef QHash<QString /*scpiEntityName*/, int /*entityId*/> ScpiEntityHash;
    typedef QHash<int /*entityId*/, QList<cSCPICmdInfoPtr>>ScpiParseInfo;

    const ScpiEntityHash &getEntitiesWithScpi() const;
    const ScpiParseInfo &getParamInfo() const;
    const ScpiParseInfo &getComponentInfo() const;
    const ScpiParseInfo &getRpcInfo() const;

private:
    ScpiEntityHash m_entitiesWithScpi;
    ScpiParseInfo m_componentInfo;
    ScpiParseInfo m_paramInfo;
    ScpiParseInfo m_rpcInfo;
};

}
#endif // VEINSCPIMODULEINTERFACEPARSER_H
