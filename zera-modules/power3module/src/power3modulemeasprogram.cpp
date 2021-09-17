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
#include "power3module.h"
#include "power3modulemeasprogram.h"
#include "power3measdelegate.h"
#include "power3moduleconfiguration.h"


namespace POWER3MODULE
{

cPower3ModuleMeasProgram::cPower3ModuleMeasProgram(cPower3Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module)
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


cPower3ModuleMeasProgram::~cPower3ModuleMeasProgram()
{
}


void cPower3ModuleMeasProgram::start()
{
}


void cPower3ModuleMeasProgram::stop()
{
}

cPower3ModuleConfigData *cPower3ModuleMeasProgram::getConfData()
{
    return qobject_cast<cPower3ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cPower3ModuleMeasProgram::generateInterface()
{
    cVeinModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < getConfData()->m_nPowerSystemCount; i++)
    {
        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_HPP%1").arg(i+1),
                                            QString("Component forwards harmonic power active values"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("P%1").arg(i+1)); // we take "system" as name because we export real- and imaginary part
        pActvalue->setUnit("W");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_HPQ%1").arg(i+1),
                                            QString("Component forwards harmonic power reactive values"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("Q%1").arg(i+1)); // we take "system" as name because we export real- and imaginary part
        pActvalue->setUnit("Var");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_HPS%1").arg(i+1),
                                            QString("Component forwards harmonic power apparent values"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("S%1").arg(i+1)); // we take "system" as name because we export real- and imaginary part
        pActvalue->setUnit("VA");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

    }

    m_pHPWCountInfo = new cVeinModuleMetaData(QString("HPWCount"), QVariant(getConfData()->m_nPowerSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pHPWCountInfo);

    m_pMeasureSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Component forwards a signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cPower3ModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;
    QList<cVeinModuleComponentInput*> inputList;

    for (int i = 0; i < getConfData()->m_nPowerSystemCount; i++)
    {
        // we first test that wanted input components exist
        if ( (m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_nModuleId, getConfData()->m_powerSystemConfigList.at(i).m_sInputU)) &&
             (m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_nModuleId, getConfData()->m_powerSystemConfigList.at(i).m_sInputI)) )
        {
            cPower3MeasDelegate* cPMD;
            cVeinModuleComponentInput *vmci;

            if (i == (getConfData()->m_nPowerSystemCount-1))
            {
                cPMD = new cPower3MeasDelegate(m_ActValueList.at(i*3), m_ActValueList.at(i*3+1), m_ActValueList.at(i*3+2),true);
                connect(cPMD, &cPower3MeasDelegate::measuring, this, &cPower3ModuleMeasProgram::setMeasureSignal);
            }
            else
                cPMD = new cPower3MeasDelegate(m_ActValueList.at(i*3), m_ActValueList.at(i*3+1), m_ActValueList.at(i*3+2));

            m_Power3MeasDelegateList.append(cPMD);

            vmci = new cVeinModuleComponentInput(getConfData()->m_nModuleId, getConfData()->m_powerSystemConfigList.at(i).m_sInputU);
            inputList.append(vmci);
            connect(vmci, &cVeinModuleComponentInput::sigValueChanged, cPMD, &cPower3MeasDelegate::actValueInput1);

            vmci = new cVeinModuleComponentInput(getConfData()->m_nModuleId, getConfData()->m_powerSystemConfigList.at(i).m_sInputI);
            inputList.append(vmci);
            connect(vmci, &cVeinModuleComponentInput::sigValueChanged, cPMD, &cPower3MeasDelegate::actValueInput2);
        }
        else
            error = true;
    }

    if (error)
        emit activationError();
    else
    {
        m_pEventSystem->setInputList(inputList);
        emit activationContinue();
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

    emit deactivationContinue();
}


void cPower3ModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


void cPower3ModuleMeasProgram::setMeasureSignal(int signal)
{
    m_pMeasureSignal->setValue(signal);
}

}



