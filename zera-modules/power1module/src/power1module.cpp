#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <veinpeer.h>
#include <veinentity.h>

#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>

#include "debug.h"
#include "power1module.h"
#include "power1moduleconfiguration.h"
#include "power1moduleconfigdata.h"
#include "power1modulemeasprogram.h"
#include "power1moduleobservation.h"
#include "moduleparameter.h"
#include "moduleinfo.h"
#include "modulesignal.h"
#include "moduleerror.h"
#include "errormessages.h"

namespace POWER1MODULE
{

cPower1Module::cPower1Module(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxy, peer, new cPower1ModuleConfiguration(), parent)
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


cPower1Module::~cPower1Module()
{
    delete m_pConfiguration;
}


QByteArray cPower1Module::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}



void cPower1Module::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cPower1Module::setupModule()
{
    cPower1ModuleConfigData* pConfData;
    pConfData = qobject_cast<cPower1ModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we need some program that does the measuring on dsp
    m_pMeasProgram = new cPower1ModuleMeasProgram(this, m_pProxy, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    // and module observation in case we have to react to naming changes
    m_pPower1ModuleObservation = new cPower1ModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pPower1ModuleObservation);
    connect(m_pPower1ModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pPower1ModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pPower1ModuleObservation, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cPower1Module::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            delete m_ModuleActivistList.at(i);
        }
        m_ModuleActivistList.clear();
    }
}


void cPower1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cPower1Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cPower1Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cPower1Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cPower1Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cPower1Module::activationFinished()
{
    // if we get informed we have to reconfigure
    connect(m_pPower1ModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(power1ModuleReconfigure()));

    QJsonObject jsonObj;

    jsonObj.insert("ModulName", getModuleName());
    jsonObj.insert("SCPIModuleName", getSCPIModuleName());
    jsonObj.insert("VeinPeer", m_pPeer->getName());

    QJsonArray jsonArr;
    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->exportInterface(jsonArr);

    jsonObj.insert("Entities", QJsonValue(jsonArr));

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);

    QByteArray ba;
    ba = jsonDoc.toBinaryData();

#ifdef DEBUG
    qDebug() << jsonDoc;
#endif

    m_pModuleInterfaceEntity->setValue(QVariant(ba), m_pPeer);

    emit activationReady();
}


void cPower1Module::deactivationStart()
{
    // if we get informed we have to reconfigure
    disconnect(m_pPower1ModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(power1ModuleReconfigure()));

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cPower1Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cPower1Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cPower1Module::deactivationFinished()
{
    emit deactivationReady();
}


void cPower1Module::power1ModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
