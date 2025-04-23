#include "burden1module.h"
#include "burden1modulemeasprogram.h"
#include "burden1measdelegate.h"
#include "burden1moduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>
#include <doublevalidator.h>
#include <scpi.h>
#include <stringvalidator.h>

namespace BURDEN1MODULE
{

cBurden1ModuleMeasProgram::cBurden1ModuleMeasProgram(cBurden1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_searchActualValuesState.addTransition(this, &cBurden1ModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, &QState::entered, this, &cBurden1ModuleMeasProgram::searchActualValues);
    connect(&m_activationDoneState, &QState::entered, this, &cBurden1ModuleMeasProgram::activateDone);

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, &cBurden1ModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, &QState::entered, this, &cBurden1ModuleMeasProgram::deactivateMeas);
    connect(&m_deactivateDoneState, &QState::entered, this, &cBurden1ModuleMeasProgram::deactivateMeasDone);
}


cBurden1ModuleMeasProgram::~cBurden1ModuleMeasProgram()
{
    // we delete burden meas delegates on deactivation of module
}

cBurden1ModuleConfigData *cBurden1ModuleMeasProgram::getConfData()
{
    return qobject_cast<cBurden1ModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}


void cBurden1ModuleMeasProgram::generateVeinInterface()
{

    for (int i = 0; i < getConfData()->m_nBurdenSystemCount; i++) {
        VfModuleComponent *pActvalue;
        QString key;
        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Burden%1").arg(i+1),
                                            QString("Burden actual value Sb"));
        pActvalue->setChannelName(QString("BRD%1").arg(i+1));
        pActvalue->setUnit("VA");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP, pActvalue->getName());
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_PFactor%1").arg(i+1),
                                            QString("Burden powerfactor cos(β)"));
        pActvalue->setChannelName(QString("POF%1").arg(i+1));
        pActvalue->setUnit("");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP, pActvalue->getName());
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                            QString("ACT_Ratio%1").arg(i+1),
                                            QString("Burden ratio value Sn"));
        pActvalue->setChannelName(QString("RAT%1").arg(i+1));
        pActvalue->setUnit("%");
        pActvalue->setScpiInfo("MEASURE", pActvalue->getChannelName(), SCPI::isCmdwP, pActvalue->getName());
        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    QString key;
    m_pNominalRangeParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                     key = QString("PAR_NominalRange"),
                                                     QString("Nominal range"),
                                                     QVariant(getConfData()->nominalRange.m_fValue));
    m_pNominalRangeParameter->setUnit(getConfData()->m_Unit);
    m_pNominalRangeParameter->setScpiInfo("CONFIGURATION","RANGE", SCPI::isQuery|SCPI::isCmdwP, "PAR_NominalRange");
    m_pNominalRangeParameter->setValidator(new cDoubleValidator(0.001, 10000.0, 0.001));
    m_pModule->m_veinModuleParameterMap[key] = m_pNominalRangeParameter; // for modules use

    m_pNominalRangeFactorParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                           key = QString("PAR_NominalRangeFactor"),
                                                           QString("Nominal range factor"),
                                                           QVariant(getConfData()->nominalRangeFactor.m_sPar));
    m_pNominalRangeFactorParameter->setScpiInfo("CONFIGURATION","RFACTOR", SCPI::isQuery|SCPI::isCmdwP, "PAR_NominalRangeFactor");
    m_pNominalRangeFactorParameter->setValidator(new cStringValidator(QString("1;sqrt(3);1/sqrt(3);1/3")));
    m_pModule->m_veinModuleParameterMap[key] = m_pNominalRangeFactorParameter; // for modules use

    m_pNominalBurdenParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                         key = QString("PAR_NominalBurden"),
                                                         QString("Nominal burden"),
                                                         QVariant(getConfData()->nominalBurden.m_fValue));
    m_pNominalBurdenParameter->setUnit("VA");
    m_pNominalBurdenParameter->setScpiInfo("CONFIGURATION","BURDEN", SCPI::isQuery|SCPI::isCmdwP, "PAR_NominalBurden");
    m_pNominalBurdenParameter->setValidator(new cDoubleValidator(0.001, 10000.0, 0.001));
    m_pModule->m_veinModuleParameterMap[key] = m_pNominalBurdenParameter; // for modules use

    m_pWireLengthParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                   key = QString("PAR_WireLength"),
                                                   QString("Wire length value"),
                                                   QVariant(getConfData()->wireLength.m_fValue));
    m_pWireLengthParameter->setUnit("m");
    m_pWireLengthParameter->setScpiInfo("CONFIGURATION","WLENGTH", SCPI::isQuery|SCPI::isCmdwP, "PAR_WireLength");
    m_pWireLengthParameter->setValidator(new cDoubleValidator(0.0, 100.0, 0.1));
    m_pModule->m_veinModuleParameterMap[key] = m_pWireLengthParameter; // for modules use

    m_pWireCrosssectionParameter = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                            key = QString("PAR_WCrosssection"),
                                                            QString("Wire crosssection value"),
                                                            QVariant(getConfData()->wireCrosssection.m_fValue));
    m_pWireCrosssectionParameter->setUnit("mm²");
    m_pWireCrosssectionParameter->setScpiInfo("CONFIGURATION","WCSection", SCPI::isQuery|SCPI::isCmdwP, "PAR_WCrosssection");
    m_pWireCrosssectionParameter->setValidator(new cDoubleValidator(0.1, 100.0, 0.1));
    m_pModule->m_veinModuleParameterMap[key] = m_pWireCrosssectionParameter; // for modules use

    m_pBRSCountInfo = new VfModuleMetaData(QString("BRSCount"), QVariant(getConfData()->m_nBurdenSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pBRSCountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));
    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}


void cBurden1ModuleMeasProgram::searchActualValues()
{
    bool error = false;
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    for (int i = 0; i < getConfData()->m_nBurdenSystemCount; i++) {
        VeinStorage::AbstractComponentPtr inputVoltageVector =
            storageDb->findComponent(getConfData()->m_nModuleId, getConfData()->m_BurdenSystemConfigList.at(i).m_sInputVoltageVector);
        VeinStorage::AbstractComponentPtr inputCurrentVector =
            storageDb->findComponent(getConfData()->m_nModuleId, getConfData()->m_BurdenSystemConfigList.at(i).m_sInputCurrentVector);
        if(inputVoltageVector && inputCurrentVector) {
            cBurden1MeasDelegate* cBMD;
            if (i == (getConfData()->m_nBurdenSystemCount-1)) {
                cBMD = new cBurden1MeasDelegate(m_veinActValueList.at(i*3), m_veinActValueList.at(i*3+1), m_veinActValueList.at(i*3+2), getConfData()->m_Unit, true);
                connect(cBMD, &cBurden1MeasDelegate::measuring, this, &cBurden1ModuleMeasProgram::setMeasureSignal);
            }
            else
                cBMD = new cBurden1MeasDelegate(m_veinActValueList.at(i*3), m_veinActValueList.at(i*3+1), m_veinActValueList.at(i*3+2), getConfData()->m_Unit);

            m_Burden1MeasDelegateList.append(cBMD);
            connect(inputVoltageVector.get(), &VeinStorage::AbstractComponent::sigValueChange,
                    cBMD, &cBurden1MeasDelegate::actValueInput1);
            connect(inputCurrentVector.get(), &VeinStorage::AbstractComponent::sigValueChange,
                    cBMD,  &cBurden1MeasDelegate::actValueInput2);
        }
        else
            error = true;
    }

    if (error)
        notifyError(confiuredVeinComponentsNotFound);
    else
        emit activationContinue();
}


void cBurden1ModuleMeasProgram::activateDone()
{
    setParameters();

    connect(m_pNominalBurdenParameter, &VfModuleParameter::sigValueChanged, this, &cBurden1ModuleMeasProgram::newNominalBurden);
    connect(m_pNominalRangeParameter, &VfModuleParameter::sigValueChanged, this, &cBurden1ModuleMeasProgram::newNominalRange);
    connect(m_pNominalRangeFactorParameter, &VfModuleParameter::sigValueChanged, this, &cBurden1ModuleMeasProgram::newNominalFactorRange);
    connect(m_pWireLengthParameter, &VfModuleParameter::sigValueChanged, this, &cBurden1ModuleMeasProgram::newWireLength);
    connect(m_pWireCrosssectionParameter, &VfModuleParameter::sigValueChanged, this, &cBurden1ModuleMeasProgram::newWireCrosssection);

    m_bActive = true;
    emit activated();
}


void cBurden1ModuleMeasProgram::deactivateMeas()
{
    m_bActive = false;

    for (int i = 0; i < m_Burden1MeasDelegateList.count(); i++)
        delete m_Burden1MeasDelegateList.at(i);

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


void cBurden1ModuleMeasProgram::newNominalRange(QVariant nr)
{
    bool ok;
    getConfData()->nominalRange.m_fValue = nr.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cBurden1ModuleMeasProgram::newNominalFactorRange(QVariant nrf)
{
    getConfData()->nominalRangeFactor.m_sPar = nrf.toString();
    setParameters();

    emit m_pModule->parameterChanged();
}


void cBurden1ModuleMeasProgram::newNominalBurden(QVariant nb)
{
    bool ok;
    getConfData()->nominalBurden.m_fValue = nb.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cBurden1ModuleMeasProgram::newWireLength(QVariant wl)
{
    bool ok;
    getConfData()->wireLength.m_fValue = wl.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cBurden1ModuleMeasProgram::newWireCrosssection(QVariant wc)
{
    bool ok;
    getConfData()->wireCrosssection.m_fValue = wc.toFloat(&ok);
    setParameters();

    emit m_pModule->parameterChanged();
}


void cBurden1ModuleMeasProgram::setParameters()
{
    // we set the parameters here
    for (int i = 0; i < m_Burden1MeasDelegateList.count(); i++)
    {
        cBurden1MeasDelegate* tmd = m_Burden1MeasDelegateList.at(i);
        tmd->setNominalBurden(getConfData()->nominalBurden.m_fValue);
        tmd->setNominalRange(getConfData()->nominalRange.m_fValue);
        tmd->setWireLength(getConfData()->wireLength.m_fValue);
        tmd->setWireCrosssection(getConfData()->wireCrosssection.m_fValue);
        tmd->setNominalRangeFactor(getConfData()->nominalRangeFactor.m_sPar);
    }
}

}



