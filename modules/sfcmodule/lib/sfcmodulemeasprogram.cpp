#include "sfcmodulemeasprogram.h"
#include "sfcmodule.h"
#include "sfcmoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace SFCMODULE
{

cSfcModuleMeasProgram::cSfcModuleMeasProgram(cSfcModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration)
    : cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()), m_pModule(module)
{
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);

    connect(&m_activationDoneState, &QState::entered, this, &cSfcModuleMeasProgram::activateDone);

    m_deactivationMachine.addState(&m_deactivateDoneState);
    m_deactivationMachine.setInitialState(&m_deactivateDoneState);

    connect(&m_deactivateDoneState, &QState::entered, this, &cSfcModuleMeasProgram::deactivateMeasDone);
}

cSfcModuleMeasProgram::~cSfcModuleMeasProgram()
{
}

void cSfcModuleMeasProgram::start()
{
}

void cSfcModuleMeasProgram::stop()
{
}

cSfcModuleConfigData *cSfcModuleMeasProgram::getConfData()
{
    return qobject_cast<cSfcModuleConfiguration *>(m_pConfiguration.get())->getConfigurationData();
}

void cSfcModuleMeasProgram::generateVeinInterface()
{
    VfModuleActvalue *pActvalue;
}

void cSfcModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}

void cSfcModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

}
