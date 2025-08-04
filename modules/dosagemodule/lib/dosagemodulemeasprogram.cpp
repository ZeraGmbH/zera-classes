#include "dosagemodulemeasprogram.h"
#include "dosagemodule.h"
#include "dosagemoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace DOSAGEMODULE
{

cDosageModuleMeasProgram::cDosageModuleMeasProgram(cDosageModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration)
    : cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()), m_pModule(module)
{
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);

    connect(&m_activationDoneState, &QState::entered, this, &cDosageModuleMeasProgram::activateDone);

    m_deactivationMachine.addState(&m_deactivateDoneState);
    m_deactivationMachine.setInitialState(&m_deactivateDoneState);

    connect(&m_deactivateDoneState, &QState::entered, this, &cDosageModuleMeasProgram::deactivateMeasDone);
}

cDosageModuleMeasProgram::~cDosageModuleMeasProgram()
{
}

void cDosageModuleMeasProgram::start()
{
}

void cDosageModuleMeasProgram::stop()
{
}

cDosageModuleConfigData *cDosageModuleMeasProgram::getConfData()
{
    return qobject_cast<cDosageModuleConfiguration *>(m_pConfiguration.get())->getConfigurationData();
}

void cDosageModuleMeasProgram::generateVeinInterface()
{

}

void cDosageModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}

void cDosageModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

}
