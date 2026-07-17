#ifndef VEINSCPIMODULEINTERFACEPARSER_H
#define VEINSCPIMODULEINTERFACEPARSER_H

#include "scpicmdinfo.h"
#include <vs_abstractdatabase.h>
#include <QMap>

namespace SCPIMODULE
{

class VeinScpiModuleInterfaceParser
{
public:
    bool parseVeinStorage(const VeinStorage::AbstractDatabase* storageDb);

    typedef QMap<QString /*scpiEntityName*/, int /*entityId*/> ScpiEntityHash;
    const ScpiEntityHash &getEntitiesWithScpi() const;

    typedef QMap<QString /*componentName*/, cSCPICmdInfoPtr> ScpiComponentParseInfo;
    typedef QMap<int /*entityId*/, ScpiComponentParseInfo>ScpiParseInfo;
    const ScpiParseInfo &getParamInfo() const;
    const ScpiParseInfo &getMeasureInfo() const;
    const ScpiParseInfo &getCatalogInfo() const;
    const ScpiParseInfo &getRpcInfo() const;

private:
    void clear();

    ScpiEntityHash m_entitiesWithScpi;

    ScpiParseInfo m_measureInfo;
    ScpiParseInfo m_paramInfo;
    ScpiParseInfo m_catalogInfo;
    ScpiParseInfo m_rpcInfo;
};

}
#endif // VEINSCPIMODULEINTERFACEPARSER_H
