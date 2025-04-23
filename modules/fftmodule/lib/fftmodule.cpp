#include "fftmodule.h"
#include "fftmoduleconfiguration.h"
#include "fftmodulemeasprogram.h"

namespace FFTMODULE
{

cFftModule::cFftModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_unique<cFftModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of fft values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cFftModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cFftModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cFftModuleMeasProgram::activated, this, &cFftModule::activationContinue);
    connect(m_pMeasProgram, &cFftModuleMeasProgram::deactivated, this, &cFftModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cFftModule::startMeas()
{
    m_pMeasProgram->start();
}

void cFftModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
