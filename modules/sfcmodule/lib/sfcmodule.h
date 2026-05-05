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

    explicit cSfcModule(const ModuleFactoryParam &moduleParam);

protected:
    cSfcModuleMeasProgram *m_pMeasProgram = nullptr;
    virtual void setupModule() override;
    virtual void startMeas() override;
    virtual void stopMeas() override;
};

}

#endif // SFCMODULE_H
