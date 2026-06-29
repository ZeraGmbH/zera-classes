#ifndef BASEMEASMODULE
#define BASEMEASMODULE

#include "basemodule.h"
#include "vfeventsytemmoduleparam.h"
#include <vs_abstracteventsystem.h>

class cBaseMeasModule : public BaseModule
{
    Q_OBJECT
public:
    cBaseMeasModule(const ModuleFactoryParam &moduleParam);
    VfEventSytemModuleParam *getValidatorEventSystem();
protected slots:
    void activationFinished() override;
};

#endif // BASEMEASMODULE

