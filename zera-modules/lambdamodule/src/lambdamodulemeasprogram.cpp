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
#include "lambdamodule.h"
#include "lambdamodulemeasprogram.h"
#include "lambdameasdelegate.h"


namespace LAMBDAMODULE
{

cLambdaModuleMeasProgram::cLambdaModuleMeasProgram(cLambdaModule* module, cLambdaModuleConfigData& configdata)
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


cLambdaModuleMeasProgram::~cLambdaModuleMeasProgram()
{
}


void cLambdaModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cLambdaModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
}


void cLambdaModuleMeasProgram::generateInterface()
{
    cVeinModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < m_ConfigData.m_nLambdaSystemCount; i++)
    {
        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_Lambda%1").arg(i+1),
                                            QString("Component forwards lambda actual values"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("Lambda%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pLAMBDACountInfo = new cVeinModuleMetaData(QString("LambdaCount"), QVariant(m_ConfigData.m_nLambdaSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pLAMBDACountInfo);

    m_pMeasureSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Component forwards a signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cLambdaModuleMeasProgram::deleteInterface()
{
}


void cLambdaModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;
    QList<cVeinModuleComponentInput*> inputList;

    for (int i = 0; i < m_ConfigData.m_nLambdaSystemCount; i++)
    {
        // we first test that wanted input components exist
        if ( (m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_lambdaSystemConfigList.at(i).m_nInputPEntity, m_ConfigData.m_lambdaSystemConfigList.at(i).m_sInputP)) &&
             (m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_lambdaSystemConfigList.at(i).m_nInputSEntity, m_ConfigData.m_lambdaSystemConfigList.at(i).m_sInputS)) )
        {
            cLambdaMeasDelegate* cLMD;
            cVeinModuleComponentInput *vmci;

            if (i == (m_ConfigData.m_nLambdaSystemCount-1))
            {
                cLMD = new cLambdaMeasDelegate(m_ActValueList.at(i), true);
                connect(cLMD, SIGNAL(measuring(int)), this, SLOT(setMeasureSignal(int)));
            }
            else
                cLMD = new cLambdaMeasDelegate(m_ActValueList.at(i));

            m_LambdaMeasDelegateList.append(cLMD);

            vmci = new cVeinModuleComponentInput(m_ConfigData.m_lambdaSystemConfigList.at(i).m_nInputPEntity, m_ConfigData.m_lambdaSystemConfigList.at(i).m_sInputP);
            inputList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cLMD, SLOT(actValueInput1(QVariant)));

            vmci = new cVeinModuleComponentInput(m_ConfigData.m_lambdaSystemConfigList.at(i).m_nInputSEntity, m_ConfigData.m_lambdaSystemConfigList.at(i).m_sInputS);
            inputList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cLMD, SLOT(actValueInput2(QVariant)));
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


void cLambdaModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}



void cLambdaModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;

    for (int i = 0; i < m_LambdaMeasDelegateList.count(); i++)
        delete m_LambdaMeasDelegateList.at(i);

    emit deactivationContinue();
}


void cLambdaModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


void cLambdaModuleMeasProgram::setMeasureSignal(int signal)
{
    m_pMeasureSignal->setValue(signal);
}

}



