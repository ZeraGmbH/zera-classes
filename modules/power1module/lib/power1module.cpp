#include "power1module.h"
#include "power1moduleconfiguration.h"
#include "power1modulemeasprogram.h"
#include "cro_systemobserverfetchtask.h"
#include <errormessages.h>

namespace POWER1MODULE
{

cPower1Module::cPower1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cPower1ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures power with configured measuring and integration modes");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cPower1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &BaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &BaseModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

TaskTemplatePtr cPower1Module::getModuleSetUpTask()
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(getSharedChannelRangeObserver());
}

void cPower1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cPower1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
