#include "blemodulemeasprogram.h"
#include "blemodule.h"
#include "blemoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace BLEMODULE
{

cBleModuleMeasProgram::cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module)
{
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);

    connect(&m_activationDoneState, &QState::entered, this, &cBleModuleMeasProgram::activateDone);

    m_deactivationMachine.addState(&m_deactivateDoneState);
    m_deactivationMachine.setInitialState(&m_deactivateDoneState);

    connect(&m_deactivateDoneState, &QState::entered, this, &cBleModuleMeasProgram::deactivateMeasDone);
}

cBleModuleMeasProgram::~cBleModuleMeasProgram()
{
}

void cBleModuleMeasProgram::start()
{
}

void cBleModuleMeasProgram::stop()
{
}

cBleModuleConfigData *cBleModuleMeasProgram::getConfData()
{
    return qobject_cast<cBleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cBleModuleMeasProgram::generateInterface()
{
    VfModuleActvalue *pActvalue;

}

void cBleModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}

void cBleModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

}
