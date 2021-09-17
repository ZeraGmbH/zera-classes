#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>

#include "basedspmeasprogram.h"
#include "sourcemoduleprogram.h"
#include "sourcemodule.h"

namespace SOURCEMODULE
{

cSourceModuleProgram::cSourceModuleProgram(cSourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module)
{
}


void cSourceModuleProgram::generateInterface()
{
    QString key;
    configuration* config = getConfigXMLWrapper();

    m_pVeinMaxCountParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                           key = QString("PAR_MaxSources"),
                                                           QString("Component for setting the modules integration time"),
                                                           QVariant(config->max_count_sources()));
    // HOW CAN WE KEEP VALIDATION PARAMETERS IN ONE PLACE??? - XSD SHOULD BE GOOD ENOUGH!!!
    m_pVeinMaxCountParameter->setValidator(new cIntValidator(0,4));
    connect(m_pVeinMaxCountParameter, &cVeinModuleParameter::sigValueChanged, this, &cSourceModuleProgram::newMaxCount);
    m_pModule->veinModuleParameterHash[key] = m_pVeinMaxCountParameter; // auto delete / meta-data / scpi
}


configuration *cSourceModuleProgram::getConfigXMLWrapper()
{
    return static_cast<cSourceModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}


void cSourceModuleProgram::newMaxCount(QVariant maxCount)
{
    getConfigXMLWrapper()->max_count_sources(maxCount.toInt());
    emit m_pModule->parameterChanged();
}

}
