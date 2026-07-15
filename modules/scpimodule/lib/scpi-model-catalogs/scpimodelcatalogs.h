#ifndef SCPIMODELCATALOGS_H
#define SCPIMODELCATALOGS_H

#include "scpimodelbase.h"
#include "scpiinterface.h"
#include "scpicmdinfo.h"
#include <QHash>
#include <QVariant>

namespace SCPIMODULE {

class cSCPIModule;
class ScpiDelegateCatalog;

class ScpiModelCatalogs : public ScpiModelBase
{
    Q_OBJECT
public:
    ScpiModelCatalogs(cSCPIModule* scpiModule);

    bool setupScpi(cSCPIInterface *scpiInterface);
    void actualizeCatalogs(const QVariant &modInterface);

private:
    void addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo);

    cSCPIModule *m_scpiModule = nullptr;
    QHash<QString, ScpiDelegateCatalog*> m_scpiCatalogDelegateHash;
};

}
#endif // SCPIMODELCATALOGS_H
