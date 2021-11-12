#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    : m_pProxy(proxy), m_pConfiguration(pConfiguration)
{
}

cBaseMeasProgram::~cBaseMeasProgram()
{
}

void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0) {
        emit activationContinue();
    }
}



