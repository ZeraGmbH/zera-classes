#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"
#include "thdnmodulemeasprogram.h"
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace THDNMODULE
{

cThdnModule::cThdnModule(MeasurementModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cThdnModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module measures thdn values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_ActivationStartState.addTransition(this, &cThdnModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cThdnModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cThdnModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cThdnModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QAbstractState::entered, this, &cThdnModule::activationStart);
    connect(&m_ActivationExecState, &QAbstractState::entered, this, &cThdnModule::activationExec);
    connect(&m_ActivationDoneState, &QAbstractState::entered, this, &cThdnModule::activationDone);
    connect(&m_ActivationFinishedState, &QAbstractState::entered, this, &cThdnModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cThdnModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cThdnModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cThdnModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cThdnModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QAbstractState::entered, this, &cThdnModule::deactivationStart);
    connect(&m_DeactivationExecState, &QAbstractState::entered, this, &cThdnModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QAbstractState::entered, this, &cThdnModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QAbstractState::entered, this, &cThdnModule::deactivationFinished);

}


QByteArray cThdnModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}



void cThdnModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cThdnModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cThdnModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cModuleActivist::activated, this, &cBaseModule::activationContinue);
    connect(m_pMeasProgram, &cModuleActivist::deactivated, this, &cBaseModule::deactivationContinue);
    connect(m_pMeasProgram, &cModuleActivist::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cThdnModule::startMeas()
{
    m_pMeasProgram->start();
}


void cThdnModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cThdnModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cThdnModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cThdnModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cThdnModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);
    // now we still have to export the json interface information

    exportMetaData();

    emit activationReady();
}


void cThdnModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cThdnModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cThdnModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cThdnModule::deactivationFinished()
{
    emit deactivationReady();
}

}
