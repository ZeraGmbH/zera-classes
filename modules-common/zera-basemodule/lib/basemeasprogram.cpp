#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    m_pConfiguration(pConfiguration)
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
}

void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0) {
        emit activationContinue();
    }
}

