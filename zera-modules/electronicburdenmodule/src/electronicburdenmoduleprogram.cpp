#include <vfmoduleactvalue.h>
#include <vfmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>

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

    cVeinModuleActvalue* pVeinAct;
    cVeinModuleParameter* pVeinParam;
    cJsonParamValidator *jsonValidator;

    m_pVeinInterface = new VeinInterface;
    // device info (Don' movit down - our clients need it first!!)
    pVeinAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                        QString("ACT_DeviceInfo"),
                                        QString("Burden info/capabiliities"),
                                        QJsonObject());
    m_pVeinInterface->setVeinDeviceInfoComponent(pVeinAct);
    m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

    pVeinAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                        QString("ACT_DeviceState"),
                                        QString("Burden status"),
                                        QJsonObject());
    m_pVeinInterface->setVeinDeviceStateComponent(pVeinAct);
    m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

    // device param
    pVeinParam = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                        key = QString("PAR_BurdenState"),
                                                        QString("Burden parameters in JSON format"),
                                                        QJsonObject());
    m_pVeinInterface->setVeinDeviceParameterComponent(pVeinParam);
    //pVeinParam->setSCPIInfo(new cSCPIInfo("CONFIGURATION","RANGE", "10", "PAR_NominalRange", "0", s));
    jsonValidator = new cJsonParamValidator();
    m_pVeinInterface->setVeinDeviceParameterValidator(jsonValidator);
    pVeinParam->setValidator(jsonValidator);
    m_pModule->veinModuleParameterHash[key] = pVeinParam; // auto delete / meta-data / scpi

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
