#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"

namespace THDNMODULE
{

cThdnModule::cThdnModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cThdnModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures thdn values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cThdnModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cThdnModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &BaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &BaseModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cThdnModule::startMeas()
{
    m_pMeasProgram->start();
}

void cThdnModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
