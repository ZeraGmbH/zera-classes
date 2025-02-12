#include "rmsmodule.h"
#include "rmsmoduleconfiguration.h"

namespace RMSMODULE
{

cRmsModule::cRmsModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cRmsModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures true rms values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cRmsModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cRmsModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &cRmsModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &cRmsModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cRmsModule::startMeas()
{
    m_pMeasProgram->start();
}

void cRmsModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
