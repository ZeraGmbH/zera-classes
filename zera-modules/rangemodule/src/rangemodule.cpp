#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <modulevalidator.h>
#include <veinmodulecomponent.h>
#include <veinmoduleerrorcomponent.h>
#include <veinmodulemetadata.h>

#include <QDebug>
#include <QByteArray>

#include "debug.h"
#include "rangemodule.h"
#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"
#include "rangemoduleobservation.h"
#include "adjustment.h"
#include "rangeobsermatic.h"

namespace RANGEMODULE
{

cRangeModule::cRangeModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
    :cBaseMeasModule(modnr, proxy, entityId, storagesystem, new cRangeModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module is responsible for range handling,\n range setting, automatic, adjustment and scaling");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);
}


cRangeModule::~cRangeModule()
{
    delete m_pConfiguration;
}


QByteArray cRangeModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


cRangeMeasChannel *cRangeModule::getMeasChannel(QString name)
{
    cRangeMeasChannel* p_rmc = 0;
    for (int i = 0; i < m_rangeMeasChannelList.count(); i++)
    {
        p_rmc =  m_rangeMeasChannelList.at(i);
        if ((p_rmc->getName()) == name)
            return p_rmc;
    }

    return p_rmc;
}


void cRangeModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cRangeModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    cBaseMeasModule::setupModule();

    cRangeModuleConfigData *pConfData;
    pConfData = qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    m_pChannelCountInfo = new cVeinModuleMetaData(QString("ChannelCount"), QVariant(pConfData->m_nChannelCount));
    veinModuleMetaDataList.append(m_pChannelCountInfo);

    m_pGroupCountInfo = new cVeinModuleMetaData(QString("GroupCount"), QVariant(pConfData->m_nGroupCount));
    veinModuleMetaDataList.append(m_pGroupCountInfo);


    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++)
    {
        bool extend;
        QString channel;

        channel = pConfData->m_senseChannelList.at(i);
        extend = pConfData->m_ExtendChannelList.contains(channel); // let's test if this channel is range extendable
        cRangeMeasChannel* pchn = new cRangeMeasChannel(m_pProxy, &(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_senseChannelList.at(i), i+1, extend);
        m_rangeMeasChannelList.append(pchn);
        m_ModuleActivistList.append(pchn);
        connect(pchn, SIGNAL(activated()), this, SIGNAL(activationContinue()));
        connect(pchn, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
        connect(pchn, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent , SLOT(setValue(QVariant)));
    }

    // we need some program that does the range handling (observation, automatic, setting and grouping)
    // it will also do the scaling job
    m_pRangeObsermatic = new cRangeObsermatic(this, m_pProxy, &(pConfData->m_DSPServerSocket), pConfData->m_GroupList, pConfData->m_senseChannelList, pConfData->m_ObsermaticConfPar);
    m_ModuleActivistList.append(m_pRangeObsermatic);
    connect(m_pRangeObsermatic, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pRangeObsermatic, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pRangeObsermatic, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    // we have to connect all cmddone from our meas channel to range obsermatic
    // this is also used for synchronizing purpose
    // additionally we connect the newrangesignal of all measchannels to range obsermatic
    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++)
    {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        connect(pchn, SIGNAL(cmdDone(quint32)), m_pRangeObsermatic, SLOT(catchChannelReply(quint32)));
        connect(pchn, SIGNAL(newRangeList()), m_pRangeObsermatic, SLOT(catchChannelNewRangeList()));
    }

    // we also need some program for adjustment
    m_pAdjustment = new cAdjustManagement(this, m_pProxy, &(pConfData->m_DSPServerSocket), &pConfData->m_PCBServerSocket, pConfData->m_senseChannelList, pConfData->m_subdcChannelList, pConfData->m_fAdjInterval);
    m_ModuleActivistList.append(m_pAdjustment);
    connect(m_pAdjustment, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pAdjustment, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pAdjustment, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    // at last we need some program that does the measuring on dsp
    m_pMeasProgram = new cRangeModuleMeasProgram(this, m_pProxy, *pConfData);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pMeasProgram, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pMeasProgram, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));
    //
    m_pRangeModuleObservation = new cRangeModuleObservation(this, m_pProxy, &(pConfData->m_PCBServerSocket));
    m_ModuleActivistList.append(m_pRangeModuleObservation);
    connect(m_pRangeModuleObservation, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pRangeModuleObservation, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pRangeModuleObservation, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cRangeModule::startMeas()
{
    m_pMeasProgram->start();
}


void cRangeModule::stopMeas()
{
    m_pMeasProgram->stop();
}


void cRangeModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cRangeModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cRangeModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cRangeModule::activationFinished()
{
    // we connect the measurement output to our adjustment module
    connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pAdjustment, SLOT(ActionHandler(QVector<float>*)));
    // and to the range obsermatic
    connect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pRangeObsermatic, SLOT(ActionHandler(QVector<float>*)));
    // we connect a signal that range has changed to measurement for synchronizing purpose
    connect(m_pRangeObsermatic->m_pRangingSignal, SIGNAL(sigValueChanged(QVariant)),  m_pMeasProgram, SLOT(syncRanging(QVariant)));

    // if we get informed we have to reconfigure
    connect(m_pRangeModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rangeModuleReconfigure()));

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cRangeModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pAdjustment, SLOT(ActionHandler(QVector<float>*)));
    disconnect(m_pMeasProgram, SIGNAL(actualValues(QVector<float>*)), m_pRangeObsermatic, SLOT(ActionHandler(QVector<float>*)));
    disconnect(m_pRangeObsermatic->m_pRangingSignal, SIGNAL(sigValueChanged(QVariant)),  m_pMeasProgram, SLOT(syncRanging(QVariant)));

    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++)
    {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        disconnect(pchn, SIGNAL(cmdDone(quint32)), m_pRangeObsermatic, SLOT(catchChannelReply(quint32)));
        disconnect(pchn, SIGNAL(newRangeList()), m_pRangeObsermatic, SLOT(catchChannelNewRangeList()));
    }

    // if we get informed we have to reconfigure
    disconnect(m_pRangeModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rangeModuleReconfigure()));

    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cRangeModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cRangeModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cRangeModule::deactivationFinished()
{
    emit deactivationReady();
}


void cRangeModule::rangeModuleReconfigure()
{
    emit sigConfiguration(); // we configure after our notifier has detected
}

}
