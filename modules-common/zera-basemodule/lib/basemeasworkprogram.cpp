#include "basemeasworkprogram.h"

cBaseMeasWorkProgram::cBaseMeasWorkProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration, QString moduleName) :
    cModuleActivist(moduleName),
    m_pConfiguration(pConfiguration)
{
}
