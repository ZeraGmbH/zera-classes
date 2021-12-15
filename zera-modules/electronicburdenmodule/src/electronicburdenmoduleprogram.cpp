#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <vf-cpp-rpc.h>

#include "basedspmeasprogram.h"
#include "electronicburdenmoduleprogram.h"
#include "electronicburdenmodule.h"

namespace ELECTRONICBURDENMODULE
{

ElectronicBurdenModuleProgram::ElectronicBurdenModuleProgram(ElectronicBurdenModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration),
    m_pModule(module)
{
}

ElectronicBurdenModuleProgram::~ElectronicBurdenModuleProgram()
{
}

void ElectronicBurdenModuleProgram::generateInterface()
{
    QString key;
    m_pVeinDemoTest = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_DemoTest"),
                                                       QString("Demo Test demonstration value"),
                                                       QVariant(int(0)));
    m_pVeinDemoTest->setValidator(new cIntValidator(0, 10));
    connect(m_pVeinDemoTest, &cVeinModuleParameter::sigValueChanged, this, &ElectronicBurdenModuleProgram::newDemoTest);
    m_pModule->veinModuleParameterHash[key] = m_pVeinDemoTest; // auto delete / meta-data / scpi
}

void ElectronicBurdenModuleProgram::newDemoTest(QVariant val)
{
    qWarning("%i",val.toInt());
}

configuration *ElectronicBurdenModuleProgram::getConfigXMLWrapper()
{
    return static_cast<ElectronicBurdenModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}

}
