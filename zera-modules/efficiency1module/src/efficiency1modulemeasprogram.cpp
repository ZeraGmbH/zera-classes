#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

#include <ve_storagesystem.h>
#include <modulevalidator.h>
#include <veinmodulecomponentinput.h>
#include <veinmoduleactvalue.h>
#include <veinmodulemetadata.h>
#include <scpiinfo.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "measmodeinfo.h"
#include "efficiency1module.h"
#include "efficiency1modulemeasprogram.h"
#include "efficiency1measdelegate.h"


namespace EFFICIENCY1MODULE
{

cEfficiency1ModuleMeasProgram::cEfficiency1ModuleMeasProgram(cEfficiency1Module* module, cEfficiency1ModuleConfigData& configdata)
    :m_pModule(module), m_ConfigData(configdata)
{
    m_searchActualValuesState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, SIGNAL(entered()), SLOT(searchActualValues()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activateDone()));

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, SIGNAL(entered()), SLOT(deactivateMeas()));
    connect(&m_deactivateDoneState, SIGNAL(entered()), SLOT(deactivateMeasDone()));
}


cEfficiency1ModuleMeasProgram::~cEfficiency1ModuleMeasProgram()
{
    for (int i = 0; i < m_Efficiency1MeasDelegateList.count(); i++)
        delete m_Efficiency1MeasDelegateList.at(i);
}


void cEfficiency1ModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cEfficiency1ModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
}


void cEfficiency1ModuleMeasProgram::generateInterface()
{
    cVeinModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;

    pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                        QString("ACT_EFC1"),
                                        QString("Component forwards efficiency actual values"),
                                        QVariant(0.0) );
    pActvalue->setChannelName(QString("EFC1"));
    pActvalue->setUnit(""); // no unit

    pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
    pActvalue->setSCPIInfo(pSCPIInfo);

    m_ActValueList.append(pActvalue); // we add the component for our measurement
    m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

    m_pEFFCountInfo = new cVeinModuleMetaData(QString("EFCCount"), QVariant(1));
    m_pModule->veinModuleMetaDataList.append(m_pEFFCountInfo);

    m_pMeasureSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Component forwards a signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cEfficiency1ModuleMeasProgram::deleteInterface()
{
}


void cEfficiency1ModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;

    for (int i = 0; i < m_ConfigData.m_PowerInputConfiguration.m_nPowerSystemCount; i++)
    {
        // we first test that wanted input components exist
        if (!m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_PowerInputConfiguration.m_nModuleId,
                                                        m_ConfigData.m_PowerInputConfiguration.powerInputList.at(i)))
            error = true;
    }

    for (int i = 0; i < m_ConfigData.m_PowerOutputConfiguration.m_nPowerSystemCount; i++)
    {
        // we first test that wanted input components exist
        if (!m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_PowerOutputConfiguration.m_nModuleId,
                                                        m_ConfigData.m_PowerOutputConfiguration.powerInputList.at(i)))
            error = true;
    }

    if (!error)
    {
        QList<cVeinModuleComponentInput*> vmciList;

        cEfficiency1MeasDelegate* cEMD;
        cVeinModuleComponentInput *vmci;

        cEMD = new cEfficiency1MeasDelegate(m_ActValueList.at(1), true);
        connect(cEMD, SIGNAL(measuring(int)), this, SLOT(setMeasureSignal(int)));

        m_Efficiency1MeasDelegateList.append(cEMD);

        for (int i = 0; i < i < m_ConfigData.m_PowerInputConfiguration.m_nPowerSystemCount; i++)
        {
            vmci = new cVeinModuleComponentInput(m_ConfigData.m_PowerInputConfiguration.m_nModuleId, m_ConfigData.m_PowerInputConfiguration.powerInputList.at(i));
            vmciList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cEMD, SLOT(actValueInput1(QVariant)));
        }

        for (int i = 0; i < i < m_ConfigData.m_PowerOutputConfiguration.m_nPowerSystemCount; i++)
        {
            vmci = new cVeinModuleComponentInput(m_ConfigData.m_PowerOutputConfiguration.m_nModuleId, m_ConfigData.m_PowerOutputConfiguration.powerInputList.at(i));
            vmciList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cEMD, SLOT(actValueInput2(QVariant)));
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



