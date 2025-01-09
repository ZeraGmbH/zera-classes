#ifndef SEC1MODULE_H
#define SEC1MODULE_H

#include <basemeasmodule.h>

namespace SEC1MODULE
{
class cSec1ModuleMeasProgram;

class cSec1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SEC1Module";
    // shortcut of scpi module name is only first 4 characters
    // so we will provide ecalculator type EC01 .. EC99
    // and each ec will have an additional scpi parent with its number 0001 .. 9999
    static constexpr const char* BaseSCPIModuleName = "EC01";

    cSec1Module(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cSec1ModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // SEC1MODULE_H
