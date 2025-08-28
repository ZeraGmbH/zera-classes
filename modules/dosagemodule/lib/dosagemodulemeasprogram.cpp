#include "dosagemodulemeasprogram.h"
#include "dosagemodule.h"
#include <intvalidator.h>
#include <boolvalidator.h>
#include "dosagemoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>
#include <scpi.h>
#include <timerfactoryqt.h>

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

    m_ActualizeTimer = TimerFactoryQt::createPeriodic(m_nActualizeTimer);
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
    QString key;

    m_pPowerDetected = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                  key = QString("ACT_PowerAboveLimit"),
                                                  QString("Energy monitoring (if > configured limit: true)"),
                                                  QVariant(qQNaN()));
    m_pPowerDetected->setValidator(new cBoolValidator());
    m_pPowerDetected->setScpiInfo("STATUS", "POWER", SCPI::isQuery, m_pPowerDetected->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pPowerDetected; // and for the modules interface
}


void cDosageModuleMeasProgram::setupMeasureProgram()
{
    for(auto &item : getConfData()->m_DosageSystemConfigList) {
        if (m_pModule->getStorageDb()->findComponent(item.m_nEntity, item.m_ComponentName) != nullptr) {
            dosagepoweranalyser dpaTemp;
            dpaTemp.m_fUpperLimit = item.m_fUpperLimit;
            dpaTemp.m_ComponentName = item.m_ComponentName;
            dpaTemp.m_nEntity = item.m_nEntity;
            dpaTemp.m_component = m_pModule->getStorageDb()->findComponent(dpaTemp.m_nEntity, dpaTemp.m_ComponentName);
            m_PowerToAnalyseList.append(dpaTemp);
            }
         else
            qWarning("Error dosagemodule: Entity (%i) or component (%s) is not available", item.m_nEntity, qPrintable(item.m_ComponentName));
    }
}


void cDosageModuleMeasProgram::activateDone()
{
    m_bActive = true;

    connect(m_ActualizeTimer.get(), &TimerTemplateQt::sigExpired, this, &cDosageModuleMeasProgram::onActualize);
    m_ActualizeTimer->start();

    emit activated();
}

void cDosageModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

void cDosageModuleMeasProgram::onActualize()
{
    float tmpVal;
    bool exceedFlag = false;

    for (int i = 0; i < m_PowerToAnalyseList.size(); i++ ) {
        tmpVal = m_PowerToAnalyseList[i].m_component->getValue().toFloat();
        if (tmpVal > m_PowerToAnalyseList[i].m_fUpperLimit) {
            exceedFlag = true;
            qInfo("Entity: %i Component: %s exceeds upper limit", m_PowerToAnalyseList[i].m_nEntity, qPrintable(m_PowerToAnalyseList[i].m_ComponentName));
            break;
        }
    }
    m_pPowerDetected->setValue(exceedFlag);
}

}
