#include "referencemodule.h"
#include "referencemoduleconfiguration.h"

namespace REFERENCEMODULE
{

cReferenceModule::cReferenceModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cReferenceModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures reference actual values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cReferenceModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cReferenceModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cReferenceModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cReferenceModule::activationContinue, &m_ActivationAdjustmentState);
    m_ActivationAdjustmentState.addTransition(this, &cReferenceModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationAdjustmentState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QStateMachine::entered, this, &cReferenceModule::activationStart);
    connect(&m_ActivationExecState, &QStateMachine::entered, this, &cReferenceModule::activationExec);
    connect(&m_ActivationAdjustmentState, &QStateMachine::entered, this, &cReferenceModule::activationAdjustment);
    connect(&m_ActivationDoneState, &QStateMachine::entered, this, &cReferenceModule::activationDone);
    connect(&m_ActivationFinishedState, &QStateMachine::entered, this, &cReferenceModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cReferenceModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cReferenceModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cReferenceModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cReferenceModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QStateMachine::entered, this, &cReferenceModule::deactivationStart);
    connect(&m_DeactivationExecState, &QStateMachine::entered, this, &cReferenceModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QStateMachine::entered, this, &cReferenceModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QStateMachine::entered, this, &cReferenceModule::deactivationFinished);
}

cReferenceModule::~cReferenceModule()
{
    delete m_pReferenceAdjustment; // not in m_ModuleActivistList
}

cReferenceMeasChannel *cReferenceModule::getMeasChannel(const QString &name)
{
    cReferenceMeasChannel* p_rmc = 0;
    for (int i = 0; i < m_ReferenceMeasChannelList.count(); i++) {
        p_rmc =  m_ReferenceMeasChannelList.at(i);
        if ((p_rmc->getMName()) == name)
            return p_rmc;
    }
    return p_rmc;
}

void cReferenceModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    cReferenceModuleConfigData *pConfData = qobject_cast<cReferenceModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // setting of mode has been done by seperate mode module
    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++) {
        const QString channelMName = pConfData->m_referenceChannelList.at(i);
        cReferenceMeasChannel* pchn = new cReferenceMeasChannel(getSharedChannelRangeObserver()->getChannel(channelMName),
                                                                getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                                getNetworkConfig()->m_tcpNetworkFactory,
                                                                getVeinModuleName());
        m_ReferenceMeasChannelList.append(pchn);
        m_ModuleActivistList.append(pchn);
        connect(pchn, &cReferenceMeasChannel::activated, this, &cReferenceModule::activationContinue);
        connect(pchn, &cReferenceMeasChannel::deactivated, this, &cReferenceModule::deactivationContinue);
    }

    // we don't actvate this per our activation loop in m_ModuleActivistList
    // instead adjustment is activated after all other activists
    // means that module emits activationReady after reference measurement adjustment is finished
    m_pReferenceAdjustment = new cReferenceAdjustment(this, pConfData);

    connect(m_pReferenceAdjustment, &cReferenceAdjustment::activated, this, &cReferenceModule::activationContinue);
    connect(m_pReferenceAdjustment, &cReferenceAdjustment::deactivated, this, &cReferenceModule::deactivationContinue);

    // we have to connect all cmddone from our reference meas channels to refernce adjustment
    for (int i = 0; i < m_ReferenceMeasChannelList.count(); i ++) {
        cReferenceMeasChannel* pchn = m_ReferenceMeasChannelList.at(i);
        connect(pchn, &cReferenceMeasChannel::cmdDone, m_pReferenceAdjustment, &cReferenceAdjustment::catchChannelReply);
    }

    // at last we need some program that does the measuring job on dsp
    m_pMeasProgram = new cReferenceModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cReferenceModuleMeasProgram::activated, this, &cReferenceModule::activationContinue);
    connect(m_pMeasProgram, &cReferenceModuleMeasProgram::deactivated, this, &cReferenceModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cReferenceModule::startMeas()
{
    m_pMeasProgram->start();
}

void cReferenceModule::stopMeas()
{
    m_pMeasProgram->stop();
}

void cReferenceModule::activationAdjustment()
{
    // we connect the measurement output to our adjustment module
    connect(m_pMeasProgram, &cReferenceModuleMeasProgram::actualValues, m_pReferenceAdjustment, &cReferenceAdjustment::ActionHandler);

    m_pReferenceAdjustment->activate();
}

void cReferenceModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, &cReferenceModuleMeasProgram::actualValues, m_pReferenceAdjustment, &cReferenceAdjustment::ActionHandler);
    for (int i = 0; i<m_ReferenceMeasChannelList.count(); i++) {
        const cReferenceMeasChannel* pchn = m_ReferenceMeasChannelList.at(i);
        disconnect(pchn, &cReferenceMeasChannel::cmdDone, m_pReferenceAdjustment, &cReferenceAdjustment::catchChannelReply);
    }

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}

}
