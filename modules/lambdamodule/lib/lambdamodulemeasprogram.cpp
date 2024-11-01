#include "lambdamodulemeasprogram.h"
#include "lambdamodule.h"
#include "lambdamoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace LAMBDAMODULE
{

cLambdaModuleMeasProgram::cLambdaModuleMeasProgram(cLambdaModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration)
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

cLambdaModuleConfigData *cLambdaModuleMeasProgram::getConfData()
{
    return qobject_cast<cLambdaModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cLambdaModuleMeasProgram::generateInterface()
{
    VfModuleActvalue *pActvalue;
    cSCPIInfo* pSCPIInfo;
    for (int i = 0; i < getConfData()->m_nLambdaSystemCount; i++) {
        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_Lambda%1").arg(i+1),
                                            QString("Actual lambda value"));
        pActvalue->setChannelName(QString("Lambda%1").arg(i+1));
        pActvalue->setUnit("");

        pSCPIInfo = new cSCPIInfo("MEASURE", pActvalue->getChannelName(), "8", pActvalue->getName(), "0", pActvalue->getUnit());
        pActvalue->setSCPIInfo(pSCPIInfo);

        m_veinLambdaActValues.append(pActvalue); // we add the component for our measurement
        m_pModule->veinModuleActvalueList.append(pActvalue); // and for the modules interface

        pActvalue = new VfModuleActvalue(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                         QString("ACT_Load%1").arg(i+1),
                                         QString("load type"));
        m_veinLoadTypeList.append(pActvalue);
        m_pModule->veinModuleComponentList.append(pActvalue);
    }

    m_pLAMBDACountInfo = new VfModuleMetaData(QString("LambdaCount"), QVariant(getConfData()->m_nLambdaSystemCount));
    m_pModule->veinModuleMetaDataList.append(m_pLAMBDACountInfo);

    m_pMeasureSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                QString("SIG_Measuring"),
                                                QString("Signal indicating measurement activity"),
                                                QVariant(0));
    m_pModule->veinModuleComponentList.append(m_pMeasureSignal);
}

void cLambdaModuleMeasProgram::searchActualValues()
{
    bool error = false;
    m_lambdaCalcDelegate = new LambdaCalcDelegate(getConfData()->m_activeMeasModeAvail, m_veinLambdaActValues, m_veinLoadTypeList);
    connect(m_lambdaCalcDelegate, &LambdaCalcDelegate::measuring, this, &cLambdaModuleMeasProgram::setMeasureSignal);
    VeinStorage::AbstractEventSystem* storage = m_pModule->getStorageSystem();
    VeinStorage::AbstractComponentPtr activeMeasModeComponent =
        storage->getComponent(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModeComponent);
    VeinStorage::AbstractComponentPtr activeMeasModePhaseComponent =
        storage->getComponent(getConfData()->m_activeMeasModeEntity, getConfData()->m_activeMeasModePhaseComponent);
    if (activeMeasModeComponent && activeMeasModePhaseComponent) {
        connect(activeMeasModeComponent.get(), &VeinStorage::AbstractComponent::sigValueChange,
                m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePowerMeasModeChange);
        m_lambdaCalcDelegate->onActivePowerMeasModeChange(activeMeasModeComponent->getValue());

        connect(activeMeasModePhaseComponent.get(), &VeinStorage::AbstractComponent::sigValueChange,
                m_lambdaCalcDelegate, &LambdaCalcDelegate::onActivePowerPhaseMaskChange);
        m_lambdaCalcDelegate->onActivePowerPhaseMaskChange(activeMeasModePhaseComponent->getValue());

        for (int i = 0; i < getConfData()->m_nLambdaSystemCount; i++) {
            if (!error) {
                VeinStorage::AbstractComponentPtr inputPComponent =
                    storage->getComponent(getConfData()->m_lambdaSystemConfigList.at(i).m_nInputPEntity, getConfData()->m_lambdaSystemConfigList.at(i).m_sInputP);
                VeinStorage::AbstractComponentPtr inputQComponent =
                    storage->getComponent(getConfData()->m_lambdaSystemConfigList.at(i).m_nInputQEntity, getConfData()->m_lambdaSystemConfigList.at(i).m_sInputQ);
                VeinStorage::AbstractComponentPtr inputSComponent =
                    storage->getComponent(getConfData()->m_lambdaSystemConfigList.at(i).m_nInputSEntity, getConfData()->m_lambdaSystemConfigList.at(i).m_sInputS);

                if (inputPComponent && inputQComponent && inputSComponent) {
                    connect(inputPComponent.get(), &VeinStorage::AbstractComponent::sigValueChange, m_lambdaCalcDelegate, [=](QVariant value) {
                        m_lambdaCalcDelegate->handleActivePowerChange(i, value);
                    });
                    connect(inputQComponent.get(), &VeinStorage::AbstractComponent::sigValueChange, m_lambdaCalcDelegate, [=](QVariant value) {
                        m_lambdaCalcDelegate->handleReactivePowerChange(i, value);
                    });
                    connect(inputSComponent.get(), &VeinStorage::AbstractComponent::sigValueChange, m_lambdaCalcDelegate, [=](QVariant value) {
                        m_lambdaCalcDelegate->handleApparentPowerChange(i, value);
                    });
                }
                else
                    error = true;
            }
        }
    }
    else
        error = true;

    if (error)
        emit activationError();
    else
        emit activationContinue();
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
