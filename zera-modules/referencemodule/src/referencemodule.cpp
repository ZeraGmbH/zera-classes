#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include "referencemodule.h"
#include "referencemoduleconfiguration.h"
#include "referencemoduleconfigdata.h"
#include "referencemeaschannel.h"
#include "referencemodulemeasprogram.h"
#include "referencemoduleobservation.h"
#include "referenceadjustment.h"


namespace REFERENCEMODULE
{

cReferenceModule::cReferenceModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, new cReferenceModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module measures reference actual values for configured channels");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_ActivationStartState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationNext()), &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationAdjustmentState);
    m_ActivationAdjustmentState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationAdjustmentState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, SIGNAL(entered()), SLOT(activationStart()));
    connect(&m_ActivationExecState, SIGNAL(entered()), SLOT(activationExec()));
    connect(&m_ActivationAdjustmentState, SIGNAL(entered()), SLOT(activationAdjustment()));
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


cReferenceModule::~cReferenceModule()
{
    delete m_pConfiguration;
}


QByteArray cReferenceModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


cReferenceMeasChannel *cReferenceModule::getMeasChannel(QString name)
{
    cReferenceMeasChannel* p_rmc = 0;
    for (int i = 0; i < m_ReferenceMeasChannelList.count(); i++)
    {
        p_rmc =  m_ReferenceMeasChannelList.at(i);
        if ((p_rmc->getName()) == name)
            return p_rmc;
    }

    return p_rmc;
}


void cReferenceModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cReferenceModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cReferenceModuleConfigData *pConfData;
    pConfData = qobject_cast<cReferenceModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    // setting of mode has been done by seperate mode module
    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++)
    {
        cReferenceMeasChannel* pchn = new cReferenceMeasChannel(m_pProxy, &(pConfData->m_RMSocket),
                                                                &(pConfData->m_PCBServerSocket),
                                                                pConfData->m_referenceChannelList.at(i), i+1);
        m_ReferenceMeasChannelList.append(pchn);
        m_ModuleActivistList.append(pchn);
        connect(pchn, SIGNAL(activated()), this, SIGNAL(activationContinue()));
        connect(pchn, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
        connect(pchn, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));
    }

    // then we need some program for adjustment
    m_pReferenceAdjustment = new cReferenceAdjustment(this, m_pProxy, pConfData);
    //m_ModuleActivistList.append(m_pReferenceAdjustment);
    // we don't actvate this per our activation loop
    // instead adjustment is activated after all other activists
    // means that module emits activationReady after reference measurement adjustment is finished

    connect(m_pReferenceAdjustment, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pReferenceAdjustment, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pReferenceAdjustment, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));


    // we have to connect all cmddone from our reference meas channels to refernce adjustment
    for (int i = 0; i < m_ReferenceMeasChannelList.count(); i ++)
    {
        cReferenceMeasChannel* pchn = m_ReferenceMeasChannelList.at(i);
        connect(pchn, SIGNAL(cmdDone(quint32)), m_pReferenceAdjustment, SLOT(catchChannelReply(quint32)));
    }

    // at last we need some program that does the measuring job on dsp
    m_pMeasProgram = new cReferenceModuleMeasProgram(this, m_pProxy, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));
    //
    m_pReferenceModuleObservation = new cReferenceModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pReferenceModuleObservation);
    connect(m_pReferenceModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pReferenceModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pReferenceModuleObservation, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cReferenceModule::startMeas()
{
    m_pMeasProgram->start();
}


void cReferenceModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cReferenceModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cReferenceModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cReferenceModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cReferenceModule::activationAdjustment()
{
    // we connect the measurement output to our adjustment module
    connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pReferenceAdjustment, SLOT(ActionHandler(QVector<float>*)));

    m_pReferenceAdjustment->activate();
}



void cReferenceModule::activationFinished()
{
    // we connect the measurement output to our adjustment module
    // connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pReferenceAdjustment, SLOT(ActionHandler(QVector<float>*)));
    // if we get informed we have to reconfigure
    connect(m_pReferenceModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(referenceModuleReconfigure()));

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cReferenceModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pReferenceAdjustment, SLOT(ActionHandler(QVector<float>*)));
    disconnect(m_pReferenceModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(referenceModuleReconfigure()));

    for (int i = 0; i < m_ReferenceMeasChannelList.count(); i ++)
    {
        cReferenceMeasChannel* pchn = m_ReferenceMeasChannelList.at(i);
        disconnect(pchn, SIGNAL(cmdDone(quint32)), m_pReferenceAdjustment, SLOT(catchChannelReply(quint32)));
    }

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cReferenceModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cReferenceModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cReferenceModule::deactivationFinished()
{
    emit deactivationReady();
}


void cReferenceModule::referenceModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
