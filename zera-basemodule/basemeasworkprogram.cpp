#include "basemeasworkprogram.h"


cBaseMeasWorkProgram::cBaseMeasWorkProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    m_pConfiguration(pConfiguration)
{
    m_pEventSystem = new cBaseModuleEventSystem();
}

cBaseMeasWorkProgram::~cBaseMeasWorkProgram()
{
    delete m_pEventSystem;
}


cBaseModuleEventSystem *cBaseMeasWorkProgram::getEventSystem()
{
    return m_pEventSystem;
}
