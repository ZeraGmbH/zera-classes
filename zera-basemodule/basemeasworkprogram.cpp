#include "basemeasworkprogram.h"


cBaseMeasWorkProgram::cBaseMeasWorkProgram()
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
