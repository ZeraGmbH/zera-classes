#include <QJsonDocument>
#include <QJsonObject>

#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "sec1module.h"
#include "sec1moduleconfiguration.h"
#include "sec1moduleconfigdata.h"
#include "sec1modulemeasprogram.h"
#include "moduleerror.h"


namespace SEC1MODULE
{

cSec1Module::cSec1Module(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer* peer, QObject *parent)
    :cBaseModule(modnr, proxy, peer, new cSec1ModuleConfiguration(), parent)
{
    m_ModuleActivistList.clear();

    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr, 2, 10, QChar('0'));

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


cSec1Module::~cSec1Module()
{
    delete m_pConfiguration;
}


QByteArray cSec1Module::getConfiguration()
{
    return m_pConfiguration->exportConfiguration();
}


void cSec1Module::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cSec1Module::setupModule()
{
    cSec1ModuleConfigData *pConfData;
    pConfData = qobject_cast<cSec1ModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // we only have this activist
    m_pMeasProgram = new cSec1ModuleMeasProgram(this, m_pProxy, m_pPeer, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QString)), errorMessage, SLOT(appendMsg(QString)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSec1Module::unsetModule()
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


void cSec1Module::startMeas()
{
    m_pMeasProgram->start();
}


void cSec1Module::stopMeas()
{
    m_pMeasProgram->stop();
}


void cSec1Module::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSec1Module::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSec1Module::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSec1Module::activationFinished()
{
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


void cSec1Module::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSec1Module::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSec1Module::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSec1Module::deactivationFinished()
{
    emit deactivationReady();
}

}
