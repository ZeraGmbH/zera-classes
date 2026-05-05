#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(const std::shared_ptr<BaseModuleConfiguration> &configuration, const QString &moduleName) :
    cModuleActivist(moduleName),
    m_pConfiguration(configuration)
{
}

void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0) {
        emit activationContinue();
    }
}

