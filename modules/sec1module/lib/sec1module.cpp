#include "sec1module.h"
#include "sec1moduleconfiguration.h"
#include "sec1modulemeasprogram.h"

namespace SEC1MODULE
{

cSec1Module::cSec1Module(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cSec1ModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a configurable error calculator");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);
}

void cSec1Module::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSec1ModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSec1ModuleMeasProgram::activated, this, &cSec1Module::activationContinue);
    connect(m_pMeasProgram, &cSec1ModuleMeasProgram::deactivated, this, &cSec1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSec1Module::startMeas()
{
    m_pMeasProgram->start();
}

void cSec1Module::stopMeas()
{
    m_pMeasProgram->stop();
}

}
