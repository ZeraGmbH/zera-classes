#include "basemeasworkprogram.h"

cBaseMeasWorkProgram::cBaseMeasWorkProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    m_pConfiguration(pConfiguration)
{
    m_pEventSystem = new VfInputComponentsEventSystem();
}

cBaseMeasWorkProgram::~cBaseMeasWorkProgram()
{
    delete m_pEventSystem;
}

VfInputComponentsEventSystem *cBaseMeasWorkProgram::getEventSystem()
{
    return m_pEventSystem;
}
