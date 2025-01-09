#include "power2module.h"
#include "power2moduleconfiguration.h"
#include "power2modulemeasprogram.h"
#include "cro_systemobserverfetchtask.h"

namespace POWER2MODULE
{

cPower2Module::cPower2Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cPower2ModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures +/- power with configured measuring and integration modes");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cPower2Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower2ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &BaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &BaseModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

TaskTemplatePtr cPower2Module::getModuleSetUpTask()
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(getSharedChannelRangeObserver());
}

void cPower2Module::startMeas()
{
    m_pMeasProgram->start();
}

void cPower2Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
