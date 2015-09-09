#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinmodulemetadata.h>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <scpiinfo.h>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

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


QByteArray cRangeModule::getConfiguration()
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
    cRangeModuleConfigData *pConfData;
    pConfData = qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    m_pModuleName = new cVeinModuleMetaData(QString("Name"), QVariant(m_sModuleName));
    veinModuleMetaDataList.append(m_pModuleName);

    m_pModuleDescription = new cVeinModuleMetaData(QString("Description"), QVariant(m_sModuleDescription));
    veinModuleMetaDataList.append(m_pModuleDescription);

    m_pChannelNrInfo = new cVeinModuleMetaData(QString("ChannelCount"), QVariant(pConfData->m_nChannelCount));
    veinModuleMetaDataList.append(m_pChannelNrInfo);

    m_pGroupNrInfo = new cVeinModuleMetaData(QString("GroupCount"), QVariant(pConfData->m_nGroupCount));
    veinModuleMetaDataList.append(m_pGroupNrInfo);


    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++)
    {
        cRangeMeasChannel* pchn = new cRangeMeasChannel(m_pProxy, &(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_senseChannelList.at(i), i+1);
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

    // we also need some program for adjustment
    m_pAdjustment = new cAdjustManagement(this, m_pProxy, &(pConfData->m_DSPServerSocket), pConfData->m_senseChannelList, pConfData->m_subdcChannelList, pConfData->m_fAdjInterval);
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


void cRangeModule::unsetModule()
{
    cBaseModule::unsetModule();
    m_rangeMeasChannelList.clear();
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

    // we have to connect all cmddone from our meas channel to range obsermatic
    // this is also used for synchronizing purpose
    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++)
    {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        connect(pchn, SIGNAL(cmdDone(quint32)), m_pRangeObsermatic, SLOT(catchChannelReply(quint32)));
    }

    // if we get informed we have to reconfigure
    connect(m_pRangeModuleObservation, SIGNAL(moduleReconfigure()), this, SLOT(rangeModuleReconfigure()));

    m_pModuleValidator->setParameterHash(veinModuleParameterHash);
    emit addEventSystem(m_pModuleValidator);

    // now we still have to export the json interface information

    QJsonObject jsonObj;
    QJsonObject jsonObj2;

    for (int i = 0; i < veinModuleMetaDataList.count(); i++)
        veinModuleMetaDataList.at(i)->exportMetaData(jsonObj2);

    jsonObj.insert("ModuleInfo", jsonObj2);

    QJsonObject jsonObj3;

    for (int i = 0; i < veinModuleComponentList.count(); i++)
        veinModuleComponentList.at(i)->exportMetaData(jsonObj3);

    for (int i = 0; i < veinModuleActvalueList.count(); i++)
        veinModuleActvalueList.at(i)->exportMetaData(jsonObj3);

    QList<QString> keyList;
    keyList = veinModuleParameterHash.keys();

    for (int i = 0; i < keyList.count(); i++)
        veinModuleParameterHash[keyList.at(i)]->exportMetaData(jsonObj3);

    jsonObj.insert("ComponentInfo", jsonObj3);


    QJsonObject jsonObj4;

    jsonObj4.insert("ModuleName", m_sSCPIModuleName);

    // and then all the command information for actual values, parameters and for add. commands without components
    for (int i = 0; i < scpiCommandList.count(); i++)
        scpiCommandList.at(i)->appendSCPIInfo(jsonObj4);

    for (int i = 0; i < veinModuleActvalueList.count(); i++)
        veinModuleActvalueList.at(i)->exportSCPIInfo(jsonObj4);

    for (int i = 0; i < keyList.count(); i++)
        veinModuleParameterHash[keyList.at(i)]->exportSCPIInfo(jsonObj4);

    jsonObj.insert("SCPIInfo", jsonObj4);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);

    QByteArray ba;
    ba = jsonDoc.toBinaryData();

#ifdef DEBUG
    qDebug() << jsonDoc;
#endif

    m_pModuleInterfaceComponent->setValue(QVariant(ba));

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
