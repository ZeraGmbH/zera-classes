#ifndef BASEMEASMODULE
#define BASEMEASMODULE

#include "basemodule.h"
#include "vfeventsytemmoduleparam.h"
#include <ve_storagesystem.h>
#include <memory>

class cBaseMeasModule : public BaseModule
{
    Q_OBJECT
public:
    cBaseMeasModule(ModuleFactoryParam moduleParam, std::shared_ptr<BaseModuleConfiguration> modcfg);
    VfEventSytemModuleParam* m_pModuleValidator;
};

#endif // BASEMEASMODULE

