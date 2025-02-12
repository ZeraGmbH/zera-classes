#include "dftmodule.h"
#include "dftmoduleconfiguration.h"
#include "dftmodulemeasprogram.h"

namespace DFTMODULE
{

cDftModule::cDftModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cDftModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured order dft values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cDftModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cDftModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cDftModuleMeasProgram::activated, this, &cDftModule::activationContinue);
    connect(m_pMeasProgram, &cDftModuleMeasProgram::deactivated, this, &cDftModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cDftModule::startMeas()
{
    m_pMeasProgram->start();
}

void cDftModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
