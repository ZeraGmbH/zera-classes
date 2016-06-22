#include <QString>
#include <QStateMachine>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

#include <ve_storagesystem.h>
#include <modulevalidator.h>
#include <doublevalidator.h>
#include <veinmodulecomponentinput.h>
#include <veinmoduleactvalue.h>
#include <veinmodulemetadata.h>
#include <veinmoduleparameter.h>
#include <scpiinfo.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "measmodeinfo.h"
#include "burden1module.h"
#include "burden1modulemeasprogram.h"
#include "burden1measdelegate.h"


namespace BURDEN1MODULE
{

cBurden1ModuleMeasProgram::cBurden1ModuleMeasProgram(cBurden1Module* module, cBurden1ModuleConfigData& configdata)
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


cBurden1ModuleMeasProgram::~cBurden1ModuleMeasProgram()
{
    for (int i = 0; i < m_Burden1MeasDelegateList.count(); i++)
        delete m_Burden1MeasDelegateList.at(i);
}


void cBurden1ModuleMeasProgram::start()
{
    connect(this, SIGNAL(actualValues(QVector<float>*)), this, SLOT(setInterfaceActualValues(QVector<float>*)));
}


void cBurden1ModuleMeasProgram::stop()
{
    disconnect(this, SIGNAL(actualValues(QVector<float>*)), this, 0);
}


void cBurden1ModuleMeasProgram::generateInterface()
{
    cVeinModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;
    QString key, s;

    for (int i = 0; i < m_ConfigData.m_nBurdenSystemCount; i++)
    {
        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_BURDEN%1").arg(i+1),
                                            QString("Component forwards burden actual value"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("BRD%1").arg(i+1));
        pActvalue->setUnit("VA");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_PFACTOR%1").arg(i+1),
                                            QString("Component forwards burden powerfactor"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("POF%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_RATIO%1").arg(i+1),
                                            QString("Component forwards Burden ratio value"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("RAT%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_ActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }


    cVeinModuleParameter* m_pNominalRangeParameter;
    cVeinModuleParameter* m_pNominalBurdenParameter;
    cVeinModuleParameter* m_pWireLengthParameter;
    cVeinModuleParameter* m_pWireCrosssectionParameter;



    m_pNominalRangeParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                        key = QString("PAR_NominalRange"),
                                                        QString("Component for setting the modules nominal range"),
                                                        QVariant(m_ConfigData.nominalRange.m_fValue));
    s = QString(m_ConfigData.m_Unit);
    m_pNominalRangeParameter->setUnit(s);
    m_pNominalRangeParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","RANGE", "10", "PAR_NominalRange", "0", s));

    cDoubleValidator *dValidator;
    dValidator = new cDoubleValidator(1.0, 300.0, 0.1);
    m_pNominalRangeParameter->setValidator(dValidator);

    m_pModule->veinModuleParameterHash[key] = m_pNominalRangeParameter; // for modules use

    m_pNominalBurdenParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                         key = QString("PAR_Burden"),
                                                         QString("Component for setting the modules nominal burden"),
                                                         QVariant(m_ConfigData.nominalBurden.m_fValue));
    s = QString("VA");
    m_pNominalBurdenParameter->setUnit(s);
    m_pNominalBurdenParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","BURDEN", "10", "PAR_Burden", "0", s));

    dValidator = new cDoubleValidator(0.1, 500.0, 0.1);
    m_pNominalBurdenParameter->setValidator(dValidator);

    m_pModule->veinModuleParameterHash[key] = m_pNominalBurdenParameter; // for modules use

    m_pWireLengthParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                      key = QString("PAR_WireLength"),
                                                      QString("Component for setting the modules wire length value"),
                                                      QVariant(m_ConfigData.wireLength.m_fValue));
    s = QString("m");
    m_pWireLengthParameter->setUnit(s);
    m_pWireLengthParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","WLENGTH", "10", "PAR_WireLength", "0", s));

    dValidator = new cDoubleValidator(0.1, 100.0, 0.1);
    m_pWireLengthParameter->setValidator(dValidator);

    m_pModule->veinModuleParameterHash[key] = m_pWireLengthParameter; // for modules use

    m_pWireCrosssectionParameter = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                            key = QString("PAR_WCrosssection"),
                                                            QString("Component for setting the modules wire crosssection value"),
                                                            QVariant(m_ConfigData.wireCrosssection.m_fValue));
    s = QString("mm²");
    m_pWireCrosssectionParameter->setUnit(s);
    m_pWireCrosssectionParameter->setSCPIInfo(new cSCPIInfo("CONFIGURATION","WCSection", "10", "PAR_WCrosssection", "0", s));

    m_pWireCrosssectionParameter->setValidator(dValidator);

    m_pModule->veinModuleParameterHash[key] = m_pWireCrosssectionParameter; // for modules use

    m_pBRSCountInfo = new cVeinModuleMetaData(QString("BRSCount"), QVariant(m_ConfigData.m_nBurdenSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pBRSCountInfo);

    m_pMeasureSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Component forwards a signal indicating measurement activity"),
                                                QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cBurden1ModuleMeasProgram::deleteInterface()
{
}


void cBurden1ModuleMeasProgram::searchActualValues()
{
    bool error;

    error = false;
    QList<cVeinModuleComponentInput*> inputList;

    for (int i = 0; i < m_ConfigData.m_nBurdenSystemCount; i++)
    {
        // we first test that wanted input components exist
        if ( (m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_nModuleId, m_ConfigData.m_BurdenSystemConfigList.at(i).m_sInputVoltageVector)) &&
             (m_pModule->m_pStorageSystem->hasStoredValue(m_ConfigData.m_nModuleId, m_ConfigData.m_BurdenSystemConfigList.at(i).m_sInputCurrentVector)) )
        {
            cBurden1MeasDelegate *cBMD;
            cVeinModuleComponentInput *vmci;

            if (i == (m_ConfigData.m_nBurdenSystemCount-1))
            {
                cBMD = new cBurden1MeasDelegate(m_ActValueList.at(i*3), m_ActValueList.at(i*3+1), m_ActValueList.at(i*3+2), m_ConfigData.m_Unit, true);
                connect(cBMD, SIGNAL(measuring(int)), this, SLOT(setMeasureSignal(int)));
            }
            else
                cBMD = new cBurden1MeasDelegate(m_ActValueList.at(i*3), m_ActValueList.at(i*3+1), m_ActValueList.at(i*3+2), m_ConfigData.m_Unit);

            vmci = new cVeinModuleComponentInput(m_ConfigData.m_nModuleId, m_ConfigData.m_BurdenSystemConfigList.at(i).m_sInputVoltageVector);
            inputList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cBMD, SLOT(actValueInput1(QVariant)));

            vmci = new cVeinModuleComponentInput(m_ConfigData.m_nModuleId, m_ConfigData.m_BurdenSystemConfigList.at(i).m_sInputCurrentVector);
            inputList.append(vmci);
            connect(vmci, SIGNAL(sigValueChanged(QVariant)), cBMD, SLOT(actValueInput2(QVariant)));
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


void cBurden1ModuleMeasProgram::activateDone()
{
    m_bActive = true;
    emit activated();
}



void cBurden1ModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;
    emit deactivationContinue();
}


void cBurden1ModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}


void cBurden1ModuleMeasProgram::setMeasureSignal(int signal)
{
    m_pMeasureSignal->setValue(signal);
}

}



