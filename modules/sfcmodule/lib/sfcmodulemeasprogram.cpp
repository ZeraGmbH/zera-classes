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
    m_pSECInterface = std::make_unique<Zera::cSECInterface>();

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
    m_pStartStopPar = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                         key = QString("PAR_StartStop"),
                                         QString("Start/stop measurement (start=1, stop=0)"),
                                         QVariant((int)0));
    m_pStartStopPar->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("START"), SCPI::isCmdwP | SCPI::isQuery, m_pStartStopPar->getName(), SCPI::isComponent));
    m_pModule->m_veinModuleParameterMap[key] = m_pStartStopPar;
    cIntValidator *iValidator;
    iValidator = new cIntValidator(0,1,1);
    m_pStartStopPar->setValidator(iValidator);

    m_pFlankCountAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("ACT_FlankCount"),
                                             QString("Measurement of flank count"),
                                             QVariant((int)0));
    m_pFlankCountAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("FLANKCOUNT"),
                                                SCPI::isQuery,
                                                m_pFlankCountAct->getName(),
                                                SCPI::isComponent));
    m_pModule->m_veinModuleParameterMap[key] = m_pFlankCountAct;

    m_pLedStateAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("ACT_LedState"),
                                             QString("Current state of scanning head LED (on=1, off=0)"),
                                             QVariant((int)0));
    m_pLedStateAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("LEDSTATUS"),
                                                SCPI::isQuery,
                                                m_pLedStateAct->getName(),
                                                SCPI::isComponent));
    m_pModule->m_veinModuleParameterMap[key] = m_pLedStateAct;

    m_pLedInitialStateAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                           key = QString("ACT_InitialLedState"),
                                           QString("Initial state of scanning head LED (on=1, off=0)"),
                                           QVariant((int)0));
    m_pLedInitialStateAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("LEDSTATUS"),
                                              SCPI::isQuery,
                                              m_pLedInitialStateAct->getName(),
                                              SCPI::isComponent));
    m_pModule->m_veinModuleParameterMap[key] = m_pLedInitialStateAct;

}

void cSfcModuleMeasProgram::activateDone()
{
    connect(m_pStartStopPar, &VfModuleParameter::sigValueChanged, this, &cSfcModuleMeasProgram::onStartStopChanged);
    m_bActive = true;
    emit activated();
}

void cSfcModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

void cSfcModuleMeasProgram::onStartStopChanged(QVariant newValue)
{
    qInfo("kick off ec flank count thingy");
}

}
