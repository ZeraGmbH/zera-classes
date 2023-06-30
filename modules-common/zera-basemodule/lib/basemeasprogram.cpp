#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
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

