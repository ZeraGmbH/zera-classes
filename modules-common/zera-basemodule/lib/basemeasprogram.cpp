#include "basemeasprogram.h"

cBaseMeasProgram::cBaseMeasProgram(const QString &moduleName) :
    cModuleActivist(moduleName)
{
}

void cBaseMeasProgram::monitorConnection()
{
    m_nConnectionCount--;
    if (m_nConnectionCount == 0)
        emit activationContinue();
}
