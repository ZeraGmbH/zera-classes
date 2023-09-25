#include "efficiency1modulemeasprogram.h"
#include "efficiency1module.h"
#include "efficiency1measdelegate.h"
#include "efficiency1moduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace EFFICIENCY1MODULE
{

cEfficiency1ModuleMeasProgram::cEfficiency1ModuleMeasProgram(cEfficiency1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module)
{
    m_searchActualValuesState.addTransition(this, &cEfficiency1ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, &QState::entered, this, &cEfficiency1ModuleMeasProgram::searchActualValues);
    connect(&m_activationDoneState, &QState::entered, this, &cEfficiency1ModuleMeasProgram::activateDone);

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, &cEfficiency1ModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, &QState::entered, this, &cEfficiency1ModuleMeasProgram::deactivateMeas);
    connect(&m_deactivateDoneState, &QState::entered, this, &cEfficiency1ModuleMeasProgram::deactivateMeasDone);
}


cEfficiency1ModuleMeasProgram::~cEfficiency1ModuleMeasProgram()
{
    for (int i = 0; i < m_Efficiency1MeasDelegateList.count(); i++)
        delete m_Efficiency1MeasDelegateList.at(i);
}


void cEfficiency1ModuleMeasProgram::start()
{
}


void cEfficiency1ModuleMeasProgram::stop()
{
}

cEfficiency1ModuleConfigData *cEfficiency1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cEfficiency1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cEfficiency1ModuleMeasProgram::generateInterface()
{
    VfModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;

    pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                        QString("ACT_EFC1"),
                                        QString("Efficiency actual values"),
                                        QVariant(0.0) );
    pActvalue->setChannelName(QString("EFC1"));
    pActvalue->setUnit(""); // no unit

    pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
    pActvalue->setSCPIInfo(pSCPIInfo);

    m_veinActValueList.append(pActvalue); // we add the component for our measurement
    m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

    m_pEFFCountInfo = new VfModuleMetaData(QString("EFCCount"), QVariant(1));
    m_pModule->veinModuleMetaDataList.append(m_pEFFCountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cEfficiency1ModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;

    for (int i = 0; i < getConfData()->m_PowerInputConfiguration.m_nPowerSystemCount; i++)
    {
        // we first test that wanted input components exist
        if (!m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_PowerInputConfiguration.m_nModuleId,
                                                        getConfData()->m_PowerInputConfiguration.powerInputList.at(i)))
            error = true;
    }

    for (int i = 0; i < getConfData()->m_PowerOutputConfiguration.m_nPowerSystemCount; i++)
    {
        // we first test that wanted input components exist
        if (!m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_PowerOutputConfiguration.m_nModuleId,
                                                        getConfData()->m_PowerOutputConfiguration.powerInputList.at(i)))
            error = true;
    }

    if (!error)
    {
        QList<VfModuleComponentInput*> vmciList;

        cEfficiency1MeasDelegate* cEMD;
        VfModuleComponentInput *vmci;

        cEMD = new cEfficiency1MeasDelegate(m_veinActValueList.at(0), true);
        connect(cEMD, &cEfficiency1MeasDelegate::measuring, this, &cEfficiency1ModuleMeasProgram::setMeasureSignal);

        m_Efficiency1MeasDelegateList.append(cEMD);

        for (int i = 0; i < getConfData()->m_PowerInputConfiguration.m_nPowerSystemCount; i++)
        {
            vmci = new VfModuleComponentInput(getConfData()->m_PowerInputConfiguration.m_nModuleId, getConfData()->m_PowerInputConfiguration.powerInputList.at(i));
            cEMD->addInputPowerValue(vmci);
            vmciList.append(vmci);
            connect(vmci, &VfModuleComponentInput::sigValueChanged, cEMD, &cEfficiency1MeasDelegate::actValueInput1);
        }

        for (int i = 0; i < getConfData()->m_PowerOutputConfiguration.m_nPowerSystemCount; i++)
        {
            vmci = new VfModuleComponentInput(getConfData()->m_PowerOutputConfiguration.m_nModuleId, getConfData()->m_PowerOutputConfiguration.powerInputList.at(i));
            cEMD->addOutputPowerValue(vmci);
            vmciList.append(vmci);
            connect(vmci, &VfModuleComponentInput::sigValueChanged, cEMD, &cEfficiency1MeasDelegate::actValueInput2);
        }

        m_pEventSystem->setInputList(vmciList);
        emit activationContinue();
    }

    else
        emit activationError();

}


void cEfficiency1ModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}



void cEfficiency1ModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;

    for (int i = 0; i < m_Efficiency1MeasDelegateList.count(); i++)
        delete m_Efficiency1MeasDelegateList.at(i);

    emit deactivationContinue();
}


void cEfficiency1ModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


void cEfficiency1ModuleMeasProgram::setMeasureSignal(int signal)
{
    m_pMeasureSignal->setValue(signal);
}

}



