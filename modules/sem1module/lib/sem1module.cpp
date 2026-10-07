#include "sem1module.h"
#include "sem1modulemeasprogram.h"
#include "unithelper.h"

namespace SEM1MODULE
{

cSem1Module::cSem1Module(const ModuleFactoryParam &moduleParam,
                         const QString &moduleDescription,
                         const QString &moduleName,
                         const QString &scpiModuleName) :
    SemSpmModule(moduleParam,
                 moduleDescription,
                 moduleName,
                 scpiModuleName)
{
}

void cSem1Module::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    // we only have this activist
    m_pMeasProgram = new cSem1ModuleMeasProgram(this, cUnitHelper::getEnergyUnitFactorHash());
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cBaseMeasProgram::activated, this, &cSem1Module::activationContinue);
    connect(m_pMeasProgram, &cBaseMeasProgram::deactivated, this, &cSem1Module::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

}
