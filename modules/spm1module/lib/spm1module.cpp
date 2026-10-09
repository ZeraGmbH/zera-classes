#include "spm1module.h"
#include "semspmmodulemeasprogram.h"
#include "functionsspm.h"

namespace SPM1MODULE
{

cSpm1Module::cSpm1Module(const ModuleFactoryParam &moduleParam,
                         const QString &moduleDescription,
                         const QString &moduleName,
                         const QString &scpiModuleName) :
    SemSpmModule(moduleParam,
                 moduleDescription,
                 moduleName,
                 scpiModuleName)
{
}

void cSpm1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new SemSpmModuleMeasProgram(this, std::make_unique<FunctionsSpm>());
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cBaseMeasProgram::activated, this, &cSpm1Module::activationContinue);
    connect(m_pMeasProgram, &cBaseMeasProgram::deactivated, this, &cSpm1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

}
