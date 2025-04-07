#include "sfcmodulemeasprogram.h"
#include "sfcmodule.h"
#include "sfcmoduleconfiguration.h"
#include <errormessages.h>
#include <intvalidator.h>
#include <reply.h>
#include <scpi.h>

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
    QString key;
    m_pStartStop = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                         key = QString("PAR_StartStop"),
                                         QString("Start/stop measurement (start=1, stop=0)"),
                                         QVariant((int)0));
    m_pStartStop->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("START"), SCPI::isCmdwP | SCPI::isQuery, m_pStartStop->getName(), SCPI::isComponent));
    m_pModule->m_veinModuleParameterMap[key] = m_pStartStop;
    cIntValidator *iValidator;
    iValidator = new cIntValidator(0,1,1);
    m_pStartStop->setValidator(iValidator);

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

void cSfcModuleMeasProgram::onStartStopChanged()
{

}

}
