#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(Zera::Proxy::cProxy* proxy)
    :m_pProxy(proxy)
{
}


cBaseMeasProgram::~cBaseMeasProgram()
{
}


void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0)
        emit activationContinue();
}



