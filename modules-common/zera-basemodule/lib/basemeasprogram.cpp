#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration, QString moduleName) :
    cModuleActivist(moduleName),
    m_pConfiguration(pConfiguration)
{
}

void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0) {
        emit activationContinue();
    }
}

