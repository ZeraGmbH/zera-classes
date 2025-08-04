#ifndef DOSAGEMODULE_H
#define DOSAGEMODULE_H

#include "dosagemodulemeasprogram.h"
#include <basemeasmodule.h>

namespace DOSAGEMODULE
{
class cDosageModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "DosageModule";
    static constexpr const char *BaseSCPIModuleName = "DOS";

    cDosageModule(ModuleFactoryParam moduleParam);

protected:
    cDosageModuleMeasProgram *m_pMeasProgram;                  // our measuring program, lets say the working horse
    virtual void setupModule();                             // after xml configuration we can setup and export our module
    virtual void startMeas();                               // we make the measuring program start here
    virtual void stopMeas();

};

}

#endif // DOSAGEMODULE_H
