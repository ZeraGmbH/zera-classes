#include "basemeasworkprogram.h"

cBaseMeasWorkProgram::cBaseMeasWorkProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    m_pConfiguration(pConfiguration)
{
    m_pEventSystem = new VeinInputComponentsEventSystem();
}

cBaseMeasWorkProgram::~cBaseMeasWorkProgram()
{
    delete m_pEventSystem;
}

VeinInputComponentsEventSystem *cBaseMeasWorkProgram::getEventSystem()
{
    return m_pEventSystem;
}
