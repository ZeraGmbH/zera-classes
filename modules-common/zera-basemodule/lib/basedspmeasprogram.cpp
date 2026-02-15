#include "basedspmeasprogram.h"
#include <proxy.h>

cBaseDspMeasProgram::cBaseDspMeasProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration, QString moduleName) :
    cBaseMeasProgram(pConfiguration, moduleName)
{
}

void cBaseDspMeasProgram::deactivateDsp()
{
    // OMG: releaseConnectionSmart releases connections for ALL interfaces
    // So this is an interim solulution - it is time to rework proxy to allow
    // interfaces to go while others keep alive
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_dspClient); // no async. messages anymore
    disconnect(m_dspInterface.get(), 0, this, 0);
}
