#ifndef SFCMODULE_H
#define SFCMODULE_H

#include "sfcmodulemeasprogram.h"
#include <basemeasmodule.h>

namespace SFCMODULE
{
class cSfcModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "SfcModule";
    static constexpr const char *BaseSCPIModuleName = "SFC";

    cSfcModule(ModuleFactoryParam moduleParam);

protected:
    cSfcModuleMeasProgram *m_pMeasProgram;                  // our measuring program, lets say the working horse
    virtual void setupModule();                             // after xml configuration we can setup and export our module
    virtual void startMeas();                               // we make the measuring program start here
    virtual void stopMeas();

};

}

#endif // SFCMODULE_H
