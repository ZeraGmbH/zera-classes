#include "oscimodule.h"
#include "oscimoduleconfiguration.h"
#include "oscimodulemeasprogram.h"

namespace OSCIMODULE
{

cOsciModule::cOsciModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cOsciModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures oscillograms for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cOsciModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cOsciModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::activated, this, &cOsciModule::activationContinue);
    connect(m_pMeasProgram, &cOsciModuleMeasProgram::deactivated, this, &cOsciModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cOsciModule::startMeas()
{
    m_pMeasProgram->start();
}

void cOsciModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
