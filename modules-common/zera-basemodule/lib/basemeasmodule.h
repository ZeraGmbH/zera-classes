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
    cBaseMeasModule(MeasurementModuleFactoryParam moduleParam, std::shared_ptr<cBaseModuleConfiguration> modcfg);
    VfEventSytemModuleParam* m_pModuleValidator;
};

#endif // BASEMEASMODULE

