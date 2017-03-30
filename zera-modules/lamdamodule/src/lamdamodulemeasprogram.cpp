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
#include "lamdamodule.h"
#include "lamdamodulemeasprogram.h"
#include "lamdameasdelegate.h"


namespace LAMDAMODULE
{

cLamdaModuleMeasProgram::cLamdaModuleMeasProgram(cLamdaModule* module, cLamdaModuleConfigData& configdata)
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


cLamdaModuleMeasProgram::~cLamdaModuleMeasProgram()
{
}


void cLamdaModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cLamdaModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
}


void cLamdaModuleMeasProgram::generateInterface()
{
    cVeinModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;

    for (int i = 0; i < m_ConfigData.m_nLamdaSystemCount; i++)
    {
        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_LAMDA%1").arg(i+1),
                                            QString("Component forwards lamda actual values"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("Lamda%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pLAMDACountInfo = new cVeinModuleMetaData(QString("LamdaCount"), QVariant(m_ConfigData.m_nLamdaSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pLAMDACountInfo);

    m_pMeasureSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Component forwards a signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cLamdaModuleMeasProgram::deleteInterface()
{
}


void cLamdaModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;
    QList<cVeinModuleComponentInput*> inputList;

    for (int i = 0; i < m_ConfigData.m_nLamdaSystemCount; i++)
    {
        // we first test that wanted input components exist
        if ( (m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_lamdaSystemConfigList.at(i).m_nInputPEntity, m_ConfigData.m_lamdaSystemConfigList.at(i).m_sInputP)) &&
             (m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_lamdaSystemConfigList.at(i).m_nInputSEntity, m_ConfigData.m_lamdaSystemConfigList.at(i).m_sInputS)) )
        {
            cLamdaMeasDelegate* cLMD;
            cVeinModuleComponentInput *vmci;

            if (i == (m_ConfigData.m_nLamdaSystemCount-1))
            {
                cLMD = new cLamdaMeasDelegate(m_ActValueList.at(i), true);
                connect(cLMD, SIGNAL(measuring(int)), this, SLOT(setMeasureSignal(int)));
            }
            else
                cLMD = new cLamdaMeasDelegate(m_ActValueList.at(i));

            m_LamdaMeasDelegateList.append(cLMD);

            vmci = new cVeinModuleComponentInput(m_ConfigData.m_lamdaSystemConfigList.at(i).m_nInputPEntity, m_ConfigData.m_lamdaSystemConfigList.at(i).m_sInputP);
            inputList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cLMD, SLOT(actValueInput1(QVariant)));

            vmci = new cVeinModuleComponentInput(m_ConfigData.m_lamdaSystemConfigList.at(i).m_nInputSEntity, m_ConfigData.m_lamdaSystemConfigList.at(i).m_sInputS);
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


void cLamdaModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}



void cLamdaModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;

    for (int i = 0; i < m_LamdaMeasDelegateList.count(); i++)
        delete m_LamdaMeasDelegateList.at(i);

    emit deactivationContinue();
}


void cLamdaModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


void cLamdaModuleMeasProgram::setMeasureSignal(int signal)
{
    m_pMeasureSignal->setValue(signal);
}

}



