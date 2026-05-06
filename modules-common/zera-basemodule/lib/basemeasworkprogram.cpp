#include "basemeasworkprogram.h"

cBaseMeasWorkProgram::cBaseMeasWorkProgram(const std::shared_ptr<BaseModuleConfiguration> &configuration,
                                           const QString &moduleName) :
    cModuleActivist(moduleName),
    m_pConfiguration(configuration)
{
}
