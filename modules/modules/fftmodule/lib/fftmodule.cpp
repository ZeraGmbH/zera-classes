#include "fftmodule.h"
#include "fftmoduleconfiguration.h"
#include "fftmoduleconfigdata.h"
#include "fftmodulemeasprogram.h"
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace FFTMODULE
{

cFftModule::cFftModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, bool demo) :
    cBaseMeasModule(modnr, entityId, storagesystem, std::shared_ptr<cBaseModuleConfiguration>(new cFftModuleConfiguration()), demo)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures configured number of fft values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationStartState.addTransition(this, &cFftModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cFftModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cFftModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cFftModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cFftModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cFftModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cFftModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cFftModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cFftModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cFftModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cFftModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cFftModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cFftModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cFftModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cFftModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cFftModule::deactivationFinished);

}


QByteArray cFftModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cFftModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cFftModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cFftModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cFftModuleMeasProgram::activated, this, &cFftModule::activationContinue);
    connect(m_pMeasProgram, &cFftModuleMeasProgram::deactivated, this, &cFftModule::deactivationContinue);
    connect(m_pMeasProgram, &cFftModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cFftModule::startMeas()
{
    m_pMeasProgram->start();
}


void cFftModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cFftModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cFftModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cFftModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cFftModule::activationFinished()
{
    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cFftModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cFftModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cFftModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cFftModule::deactivationFinished()
{
    emit deactivationReady();
}

}
