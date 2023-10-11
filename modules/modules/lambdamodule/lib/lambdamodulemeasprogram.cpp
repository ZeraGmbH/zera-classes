#include "lambdamodulemeasprogram.h"
#include "lambdamodule.h"
#include "lambdameasdelegate.h"
#include "lambdamoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace LAMBDAMODULE
{

cLambdaModuleMeasProgram::cLambdaModuleMeasProgram(cLambdaModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration)
    :cBaseMeasWorkProgram(pConfiguration), m_pModule(module)
{
    m_searchActualValuesState.addTransition(this, &cLambdaModuleMeasProgram::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_searchActualValuesState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_searchActualValuesState);

    connect(&m_searchActualValuesState, &QState::entered, this, &cLambdaModuleMeasProgram::searchActualValues);
    connect(&m_activationDoneState, &QState::entered, this, &cLambdaModuleMeasProgram::activateDone);

    // setting up statemachine deactivation
    m_deactivateState.addTransition(this, &cLambdaModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);

    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.addState(&m_deactivateDoneState);

    m_deactivationMachine.setInitialState(&m_deactivateState);

    connect(&m_deactivateState, &QState::entered, this, &cLambdaModuleMeasProgram::deactivateMeas);
    connect(&m_deactivateDoneState, &QState::entered, this, &cLambdaModuleMeasProgram::deactivateMeasDone);
}

cLambdaModuleMeasProgram::~cLambdaModuleMeasProgram()
{
}

void cLambdaModuleMeasProgram::start()
{
}

void cLambdaModuleMeasProgram::stop()
{
}

cLambdaModuleConfigData *cLambdaModuleMeasProgram::getConfData()
{
    return qobject_cast<cLambdaModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cLambdaModuleMeasProgram::generateInterface()
{
    VfModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;
    for (int i = 0; i < getConfData()->m_nLambdaSystemCount; i++) {
        pActvalue = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_Lambda%1").arg(i+1),
                                            QString("Actual lambda value"),
                                            QVariant(0.0) );
        pActvalue->setChannelName(QString("Lambda%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinActValueList.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface
    }

    m_pLAMBDACountInfo = new VfModuleMetaData(QString("LambdaCount"), QVariant(getConfData()->m_nLambdaSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pLAMBDACountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));
    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}

void cLambdaModuleMeasProgram::searchActualValues()
{
    bool error = false;
    QList<VfModuleComponentInput*> inputList;

    if (getConfData()->m_activeMeasModeAvail) {
        if ((!m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModeComponent)) ||
            (!m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModePhaseComponent)))
            error = true;
    }

    if (!error) {
        for (int i = 0; i < getConfData()->m_nLambdaSystemCount; i++) {
            // we first test that wanted input components exist
            if (!(m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_lambdaSystemConfigList.at(i).m_nInputPEntity, getConfData()->m_lambdaSystemConfigList.at(i).m_sInputP)) &&
                 (m_pModule->m_pStorageSystem->hasStoredValue(getConfData()->m_lambdaSystemConfigList.at(i).m_nInputSEntity, getConfData()->m_lambdaSystemConfigList.at(i).m_sInputS)) )
                error = true;
        }
    }

    if (!error) {
        m_lambdaCalcDelegate = new LambdaCalcDelegate(getConfData()->m_activeMeasModeAvail);

        VfModuleComponentInput *inputPComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(0).m_nInputPEntity, getConfData()->m_lambdaSystemConfigList.at(0).m_sInputP);
        inputList.append(inputPComponent);
        connect(inputPComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePower1Change);
        VfModuleComponentInput *inputSComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(0).m_nInputSEntity, getConfData()->m_lambdaSystemConfigList.at(0).m_sInputS);
        inputList.append(inputSComponent);
        connect(inputSComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onApparentPower1Change);

        inputPComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(1).m_nInputPEntity, getConfData()->m_lambdaSystemConfigList.at(1).m_sInputP);
        inputList.append(inputPComponent);
        connect(inputPComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePower2Change);
        inputSComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(1).m_nInputSEntity, getConfData()->m_lambdaSystemConfigList.at(1).m_sInputS);
        inputList.append(inputSComponent);
        connect(inputSComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onApparentPower2Change);

        inputPComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(2).m_nInputPEntity, getConfData()->m_lambdaSystemConfigList.at(2).m_sInputP);
        inputList.append(inputPComponent);
        connect(inputPComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePower3Change);
        inputSComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(2).m_nInputSEntity, getConfData()->m_lambdaSystemConfigList.at(2).m_sInputS);
        inputList.append(inputSComponent);
        connect(inputSComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onApparentPower3Change);

        inputPComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(3).m_nInputPEntity, getConfData()->m_lambdaSystemConfigList.at(3).m_sInputP);
        inputList.append(inputPComponent);
        connect(inputPComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePowerSumChange);
        inputSComponent = new VfModuleComponentInput(getConfData()->m_lambdaSystemConfigList.at(3).m_nInputSEntity, getConfData()->m_lambdaSystemConfigList.at(3).m_sInputS);
        inputList.append(inputSComponent);
        connect(inputSComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onApparentPowerSumChange);

        VfModuleComponentInput *activeMeasModeComponent = new VfModuleComponentInput(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModeComponent);
        inputList.append(activeMeasModeComponent);
        connect(activeMeasModeComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePowerMeasModeChange);
        m_lambdaCalcDelegate->onActivePowerMeasModeChange(m_pModule->m_pStorageSystem->getStoredValue(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModeComponent));

        VfModuleComponentInput *activeMeasModePhaseComponent = new VfModuleComponentInput(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModePhaseComponent);
        inputList.append(activeMeasModePhaseComponent);
        connect(activeMeasModePhaseComponent, &VfModuleComponentInput::sigValueChanged, m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePowerPhaseMaskChange);
        m_lambdaCalcDelegate->onActivePowerPhaseMaskChange(m_pModule->m_pStorageSystem->getStoredValue(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModePhaseComponent));
    }

    if (error)
        emit activationError();
    else {
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
    delete m_lambdaCalcDelegate;
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
