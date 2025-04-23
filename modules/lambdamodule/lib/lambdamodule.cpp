#include "lambdamodule.h"
#include "lambdamoduleconfiguration.h"

namespace LAMBDAMODULE
{

cLambdaModule::cLambdaModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam,std::make_shared<cLambdaModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures configured number of harmonic power values from configured input values");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

void cLambdaModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());

    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cLambdaModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cLambdaModuleMeasProgram::activated, this, &cLambdaModule::activationContinue);
    connect(m_pMeasProgram, &cLambdaModuleMeasProgram::deactivated, this, &cLambdaModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cLambdaModule::startMeas()
{
    m_pMeasProgram->start();
}

void cLambdaModule::stopMeas()
{
    m_pMeasProgram->stop();
}

}
