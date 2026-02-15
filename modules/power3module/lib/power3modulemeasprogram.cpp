#include "power3modulemeasprogram.h"
#include "power3module.h"
#include "power3measdelegate.h"
#include "power3moduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>
#include <scpi.h>

namespace POWER3MODULE
{

cPower3ModuleMeasProgram::cPower3ModuleMeasProgram(cPower3Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_searchActualValuesState.addTransition(this, &cPower3ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, &QState::entered, this, &cPower3ModuleMeasProgram::searchActualValues);
    connect(&m_activationDoneState, &QState::entered, this, &cPower3ModuleMeasProgram::activateDone);

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, &cPower3ModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, &QState::entered, this, &cPower3ModuleMeasProgram::deactivateMeas);
    connect(&m_deactivateDoneState, &QState::entered, this, &cPower3ModuleMeasProgram::deactivateMeasDone);
}

cPower3ModuleConfigData *cPower3ModuleMeasProgram::getConfData()
{
    return qobject_cast<cPower3ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cPower3ModuleMeasProgram::generateVeinInterface()
{
    for (int i = 0; i < getConfData()->m_nPowerSystemCount; i++) {
        VfModuleComponent *pActvalue;
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_HPP%1").arg(i+1),
                                            QString("Harmonic power active values"));
        pActvalue->setChannelName(QString("P%1").arg(i+1)); // we take "system" as name because we export real- and imaginary part
        pActvalue->setUnit("W");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_HPQ%1").arg(i+1),
                                            QString("Harmonic power reactive values"));
        pActvalue->setChannelName(QString("Q%1").arg(i+1)); // we take "system" as name because we export real- and imaginary part
        pActvalue->setUnit("Var");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_HPS%1").arg(i+1),
                                            QString("Harmonic power apparent values"));
        pActvalue->setChannelName(QString("S%1").arg(i+1)); // we take "system" as name because we export real- and imaginary part
        pActvalue->setUnit("VA");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP);
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->m_veinComponentsWithMetaAndScpi.append(pActvalue); // and for the modules interface
    }
    m_pHPWCountInfo = new VfModuleMetaData(QString("HPWCount"), QVariant(getConfData()->m_nPowerSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pHPWCountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->m_veinComponentsWithMetaAndScpi.append(m_pMeasureSignal);
}

void cPower3ModuleMeasProgram::searchActualValues()
{
    bool error = false;
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    m_fftSignal = storageDb->findComponent(getConfData()->m_nModuleId, "SIG_Measuring");
    if (m_fftSignal) {
        connect(m_fftSignal.get(), &VeinStorage::AbstractComponent::sigValueChange,
                this, &cPower3ModuleMeasProgram::onFftSigChange);
        for (int i = 0; i < getConfData()->m_nPowerSystemCount; i++) {
            VeinStorage::AbstractComponentPtr inputU =
                storageDb->findComponent(getConfData()->m_nModuleId, getConfData()->m_powerSystemConfigList.at(i).m_sInputU);
            VeinStorage::AbstractComponentPtr inputI =
                storageDb->findComponent(getConfData()->m_nModuleId, getConfData()->m_powerSystemConfigList.at(i).m_sInputI);

            if (inputU && inputI) {
                cPower3MeasDelegate* cPMD;
                cPMD = new cPower3MeasDelegate(inputU,
                                               inputI,
                                               m_veinActValueList.at(i*3),
                                               m_veinActValueList.at(i*3+1),
                                               m_veinActValueList.at(i*3+2));
                m_Power3MeasDelegateList.append(cPMD);
            }
            else
                error = true;
        }
    }
    else
        error = true;

    if (error)
        notifyError(confiuredVeinComponentsNotFound);
    else
        emit activationContinue();
}

void cPower3ModuleMeasProgram::onFftSigChange(const QVariant &value)
{
    if (value.toInt() == 1) {
        m_pMeasureSignal->setValue(0);
        for (int i=0; i<m_Power3MeasDelegateList.count(); ++i)
            m_Power3MeasDelegateList[i]->computeOutput();
        m_pMeasureSignal->setValue(1);
    }
}

void cPower3ModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}

void cPower3ModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;
    for (int i = 0; i < m_Power3MeasDelegateList.count(); i++)
        delete m_Power3MeasDelegateList.at(i);
    m_Power3MeasDelegateList.clear();
    emit deactivationContinue();
}

void cPower3ModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

}
