#ifndef BASEMEASMODULE
#define BASEMEASMODULE

#include "modulevalidator.h"
#include <ve_storagesystem.h>
#include <memory>

class cBaseMeasModule : public cBaseModule
{
    Q_OBJECT
public:
    cBaseMeasModule(quint8 modnr,
                    Zera::Proxy::cProxy* proxy,
                    int entityId,
                    VeinEvent::StorageSystem* storagesystem,
                    std::shared_ptr<cBaseModuleConfiguration> modcfg,
                    QObject *parent = 0);
    cModuleValidator* m_pModuleValidator;
};

#endif // BASEMEASMODULE

