#include "sourcemoduleprogram.h"

namespace SOURCEMODULE
{

cSourceModuleProgram::cSourceModuleProgram(cSourceModule* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasProgram(proxy, pConfiguration), m_pModule(module)
{

}

cSourceModuleProgram::~cSourceModuleProgram()
{

}

void cSourceModuleProgram::generateInterface()
{

}

void cSourceModuleProgram::deleteInterface()
{

}

void cSourceModuleProgram::start()
{

}

void cSourceModuleProgram::stop()
{

}

}
