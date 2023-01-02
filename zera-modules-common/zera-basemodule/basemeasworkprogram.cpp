#include "basemeasworkprogram.h"

cBaseMeasWorkProgram::cBaseMeasWorkProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    m_pConfiguration(pConfiguration)
{
    m_pEventSystem = new VfEventSystemInputComponents();
}

cBaseMeasWorkProgram::~cBaseMeasWorkProgram()
{
    delete m_pEventSystem;
}

VfEventSystemInputComponents *cBaseMeasWorkProgram::getEventSystem()
{
    return m_pEventSystem;
}
