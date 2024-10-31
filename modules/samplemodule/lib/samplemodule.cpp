#include "samplemodule.h"
#include "samplemoduleconfiguration.h"
#include "samplemoduleconfigdata.h"
#include "samplechannel.h"
#include "pllmeaschannel.h"
#include "samplemodulemeasprogram.h"
#include "pllobsermatic.h"
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>

namespace SAMPLEMODULE
{

cSampleModule::cSampleModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<BaseModuleConfiguration>(new cSampleModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for pll range setting\n, pll channel selection and automatic");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cSampleModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cSampleModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cSampleModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cSampleModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cSampleModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cSampleModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cSampleModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cSampleModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cSampleModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cSampleModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cSampleModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cSampleModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cSampleModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cSampleModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cSampleModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cSampleModule::deactivationFinished);

}


QByteArray cSampleModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


cPllMeasChannel* cSampleModule::getPllMeasChannel(QString name)
{
    cPllMeasChannel* p_pllmc = 0;
    for (int i = 0; i < m_pllMeasChannelList.count(); i++)
    {
        p_pllmc =  m_pllMeasChannelList.at(i);
        if ((p_pllmc->getName()) == name)
            return p_pllmc;
    }

    return p_pllmc;
}


void cSampleModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cSampleModuleConfigData *pConfData;
    pConfData = qobject_cast<cSampleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // first we build a list of our pll meas channels, that hold informations for other activists
    for (int i = 0; i < pConfData->m_ObsermaticConfPar.m_npllChannelCount; i ++) {
        cPllMeasChannel* pllchn = new cPllMeasChannel(getNetworkConfig()->m_rmServiceConnectionInfo,
                                                      getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                      getNetworkConfig()->m_tcpNetworkFactory,
                                                      pConfData->m_ObsermaticConfPar.m_pllChannelList.at(i),
                                                      i+1);
        m_pllMeasChannelList.append(pllchn);
        m_ModuleActivistList.append(pllchn);
        connect(pllchn, &cPllMeasChannel::activated, this, &cSampleModule::activationContinue);
        connect(pllchn, &cPllMeasChannel::deactivated, this, &cSampleModule::deactivationContinue);
        connect(pllchn, &cPllMeasChannel::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);
    }

    // next we instantiate an object of sample channel so we can switch sample frequency ranges
    cSampleChannel* schn = new cSampleChannel(this, *pConfData, 1);

    m_sampleChannelList.append(schn); // we hold a list although we only have 1
    m_ModuleActivistList.append(schn);
    connect(schn, &cSampleChannel::activated, this, &cSampleModule::activationContinue);
    connect(schn, &cSampleChannel::deactivated, this, &cSampleModule::deactivationContinue);
    connect(schn, &cSampleChannel::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    // we need some program that does the pll handling (observation, automatic, setting)
    m_pPllObsermatic = new cPllObsermatic(this, *pConfData);
    m_ModuleActivistList.append(m_pPllObsermatic);
    connect(m_pPllObsermatic, &cPllObsermatic::activated, this, &cSampleModule::activationContinue);
    connect(m_pPllObsermatic, &cPllObsermatic::deactivated, this, &cSampleModule::deactivationContinue);
    connect(m_pPllObsermatic, &cPllObsermatic::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    // at last we need some program that does the measuring on dsp
    m_pMeasProgram = new cSampleModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::activated, this, &cSampleModule::activationContinue);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::deactivated, this, &cSampleModule::deactivationContinue);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSampleModule::startMeas()
{
    m_pMeasProgram->start();
}


void cSampleModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cSampleModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSampleModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSampleModule::activationDone()
{
    m_nActivationIt++;
    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSampleModule::activationFinished()
{
    // we connect the measurement output to our pll obsermatic module
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::actualValues, m_pPllObsermatic, &cPllObsermatic::ActionHandler);

    // we have to connect all cmddone from our pll meas channel to pll obsermatic
    // this is also used for synchronizing purpose
    for (int i = 0; i < m_pllMeasChannelList.count(); i ++) {
        cPllMeasChannel* pllchn = m_pllMeasChannelList.at(i);
        connect(pllchn, &cPllMeasChannel::cmdDone, m_pPllObsermatic, &cPllObsermatic::catchChannelReply);
    }

    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);
    // now we still have to export the json interface information
    exportMetaData();
    emit activationReady();
}


void cSampleModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, &cSampleModuleMeasProgram::actualValues, m_pPllObsermatic, &cPllObsermatic::ActionHandler);

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSampleModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSampleModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSampleModule::deactivationFinished()
{
    emit deactivationReady();
}

}
