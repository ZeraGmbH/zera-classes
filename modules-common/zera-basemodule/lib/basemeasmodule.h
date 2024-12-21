#ifndef BASEMEASMODULE
#define BASEMEASMODULE

#include "basemodule.h"
#include "vfeventsytemmoduleparam.h"
#include <vs_abstracteventsystem.h>
#include <memory>

class cBaseMeasModule : public BaseModule
{
    Q_OBJECT
public:
    cBaseMeasModule(ModuleFactoryParam moduleParam, std::shared_ptr<BaseModuleConfiguration> modcfg);
    VfEventSytemModuleParam* m_pModuleValidator;
protected slots:
    void activationFinished() override;
};

#endif // BASEMEASMODULE

