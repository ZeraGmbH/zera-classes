#ifndef BASEMEASMODULE
#define BASEMEASMODULE

#include "basemodule.h"
#include "vfeventsytemmoduleparam.h"
#include <ve_storagesystem.h>
#include <memory>

class cBaseMeasModule : public cBaseModule
{
    Q_OBJECT
public:
    cBaseMeasModule(quint8 modnr,
                    int entityId,
                    VeinEvent::StorageSystem* storagesystem,
                    std::shared_ptr<cBaseModuleConfiguration> modcfg,
                    bool demo);
    VfEventSytemModuleParam* m_pModuleValidator;
};

#endif // BASEMEASMODULE

