#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinentity.h>
#include <veinpeer.h>

#include "scpimodule.h"
#include "scpimoduleconfiguration.h"
#include "scpimoduleconfigdata.h"
#include "scpiserver.h"
#include "moduleerror.h"


namespace SCPIMODULE
{

cSCPIModule::cSCPIModule(quint8 modnr, Zera::Proxy::cProxy *proxi, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxi, peer, new cSCPIModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ModuleActivistList.clear();

    m_ActivationStartState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationNext()), &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, SIGNAL(entered()), SLOT(activationStart()));
    connect(&m_ActivationExecState, SIGNAL(entered()), SLOT(activationExec()));
    connect(&m_ActivationDoneState, SIGNAL(entered()), SLOT(activationDone()));
    connect(&m_ActivationFinishedState, SIGNAL(entered()), SLOT(activationFinished()));

    m_DeactivationStartState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationNext()), &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, SIGNAL(entered()), SLOT(deactivationStart()));
    connect(&m_DeactivationExecState, SIGNAL(entered()), SLOT(deactivationExec()));
    connect(&m_DeactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
    connect(&m_DeactivationFinishedState, SIGNAL(entered()), SLOT(deactivationFinished()));
}


cSCPIModule::~cSCPIModule()
{
    delete m_pConfiguration;
}


QByteArray cSCPIModule::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}


void cSCPIModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cSCPIModule::setupModule()
{
    cSCPIModuleConfigData *pConfData;
    pConfData = qobject_cast<cSCPIModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we only have this activist
    m_pSCPIServer = new cSCPIServer(this, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pSCPIServer);
    connect(m_pSCPIServer, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pSCPIServer, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pSCPIServer, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSCPIModule::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            delete m_ModuleActivistList.at(i);
        }

        m_ModuleActivistList.clear();
        if (errorMessage) delete errorMessage;
    }
}


void cSCPIModule::startMeas()
{
    // we don't start the interface expl.
}


void cSCPIModule::stopMeas()
{
    // we don't stop the interface expl.
}


void cSCPIModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSCPIModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSCPIModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSCPIModule::activationFinished()
{
    // we only have to signal activationReady
    emit activationReady();
}


void cSCPIModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSCPIModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSCPIModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSCPIModule::deactivationFinished()
{
    emit deactivationReady();
}

}
