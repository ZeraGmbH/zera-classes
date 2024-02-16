#include "rangemodule.h"
#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include "rangemodulemeasprogram.h"
#include "rangeobsermatic.h"
#include "adjustment.h"
#include <vfmodulecomponent.h>
#include <vfmoduleerrorcomponent.h>
#include <vfmodulemetadata.h>
#include <QDebug>
#include <QByteArray>

namespace RANGEMODULE
{

cRangeModule::cRangeModule(MeasurementModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cRangeModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for range handling,\n range setting, automatic, adjustment and scaling");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
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
    pConfData = qobject_cast<cRangeModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    m_pChannelCountInfo = new VfModuleMetaData(QString("ChannelCount"), QVariant(pConfData->m_nChannelCount));
    veinModuleMetaDataList.append(m_pChannelCountInfo);

    m_pGroupCountInfo = new VfModuleMetaData(QString("GroupCount"), QVariant(pConfData->m_nGroupCount));
    veinModuleMetaDataList.append(m_pGroupCountInfo);


    // first we build a list of our meas channels
    for (int i = 0; i < pConfData->m_nChannelCount; i ++)
    {
        QString channel;

        channel = pConfData->m_senseChannelList.at(i);
        cRangeMeasChannel* pchn = new cRangeMeasChannel(&(pConfData->m_RMSocket),
                                                        &(pConfData->m_PCBServerSocket),
                                                        pConfData->m_senseChannelList.at(i),
                                                        i+1,
                                                        pConfData->m_session.m_sPar,
                                                        getDemo());
        m_rangeMeasChannelList.append(pchn);
        m_ModuleActivistList.append(pchn);
        connect(pchn, &cRangeMeasChannel::activated, this, &cRangeModule::activationContinue);
        connect(pchn, &cRangeMeasChannel::deactivated, this, &cRangeModule::deactivationContinue);
        connect(pchn, &cRangeMeasChannel::errMsg, m_pModuleErrorComponent , &VfModuleErrorComponent::setValue);
    }

    // we need some program that does the range handling (observation, automatic, setting and grouping)
    // it will also do the scaling job
    m_pRangeObsermatic = new cRangeObsermatic(this,
                                              &(pConfData->m_DSPServerSocket),
                                              pConfData->m_GroupList,
                                              pConfData->m_senseChannelList,
                                              pConfData->m_ObsermaticConfPar,
                                              getDemo());
    m_ModuleActivistList.append(m_pRangeObsermatic);
    connect(m_pRangeObsermatic, &cRangeObsermatic::activated, this, &cRangeModule::activationContinue);
    connect(m_pRangeObsermatic, &cRangeObsermatic::deactivated, this, &cRangeModule::deactivationContinue);
    connect(m_pRangeObsermatic, &cRangeObsermatic::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    // we have to connect all cmddone from our meas channel to range obsermatic
    // this is also used for synchronizing purpose
    // additionally we connect the newrangesignal of all measchannels to range obsermatic
    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++)
    {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        connect(pchn, &cRangeMeasChannel::cmdDone, m_pRangeObsermatic, &cRangeObsermatic::catchChannelReply);
    }

    if(!getDemo()) {
        // we also need some program for adjustment
        m_pAdjustment = new cAdjustManagement(this, &(pConfData->m_DSPServerSocket), &pConfData->m_PCBServerSocket, pConfData->m_senseChannelList, pConfData->m_subdcChannelList, pConfData->m_fAdjInterval);
        m_ModuleActivistList.append(m_pAdjustment);
        connect(m_pAdjustment, &cAdjustManagement::activated, this, &cRangeModule::activationContinue);
        connect(m_pAdjustment, &cAdjustManagement::deactivated, this, &cRangeModule::deactivationContinue);
        connect(m_pAdjustment, &cAdjustManagement::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);
    }

    // at last we need some program that does the measuring on dsp
    m_pMeasProgram = new cRangeModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cRangeModuleMeasProgram::activated, this, &cRangeModule::activationContinue);
    connect(m_pMeasProgram, &cRangeModuleMeasProgram::deactivated, this, &cRangeModule::deactivationContinue);
    connect(m_pMeasProgram, &cRangeModuleMeasProgram::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

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
    // set new actual values PEAK,RMS,FREQ in channel class
    connect(m_pMeasProgram, &cRangeModuleMeasProgram::actualValues, this, &cRangeModule::setPeakRmsAndFrequencyValues);
    if(m_pAdjustment)
        // we connect the measurement output to our adjustment module
        connect(m_pMeasProgram, &cRangeModuleMeasProgram::actualValues, m_pAdjustment, &cAdjustManagement::ActionHandler);
    // and to the range obsermatic
    connect(m_pMeasProgram, &cRangeModuleMeasProgram::actualValues, m_pRangeObsermatic, &cRangeObsermatic::ActionHandler);
    // we connect a signal that range has changed to measurement for synchronizing purpose
    connect(m_pRangeObsermatic->m_pRangingSignal, &VfModuleComponent::sigValueChanged, m_pMeasProgram, &cRangeModuleMeasProgram::syncRanging);

    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++) {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        connect(pchn, &cRangeMeasChannel::newRangeList, m_pRangeObsermatic, &cRangeObsermatic::catchChannelNewRangeList);
    }
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // now we still have to export the json interface information
    exportMetaData();

    emit activationReady();
}


void cRangeModule::deactivationStart()
{
    // we first disconnect all what we connected when activation took place
    disconnect(m_pMeasProgram, &cRangeModuleMeasProgram::actualValues, this, &cRangeModule::setPeakRmsAndFrequencyValues);
    if(m_pAdjustment)
        disconnect(m_pMeasProgram, &cRangeModuleMeasProgram::actualValues, m_pAdjustment, &cAdjustManagement::ActionHandler);
    disconnect(m_pMeasProgram, &cRangeModuleMeasProgram::actualValues, m_pRangeObsermatic, &cRangeObsermatic::ActionHandler);
    disconnect(m_pRangeObsermatic->m_pRangingSignal, &VfModuleComponent::sigValueChanged, m_pMeasProgram, &cRangeModuleMeasProgram::syncRanging);

    for (int i = 0; i < m_rangeMeasChannelList.count(); i ++) {
        cRangeMeasChannel* pchn = m_rangeMeasChannelList.at(i);
        disconnect(pchn, &cRangeMeasChannel::cmdDone, m_pRangeObsermatic, &cRangeObsermatic::catchChannelReply);
        disconnect(pchn, &cRangeMeasChannel::newRangeList, m_pRangeObsermatic, &cRangeObsermatic::catchChannelNewRangeList);
    }

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

void cRangeModule::setPeakRmsAndFrequencyValues(const QVector<float>* const values)
{
    /* values:
     * 0              upto maxChannel-1:   Peak values
     * maxChannel     upto 2*maxchannel-1: RMS values
     * 2*maxchannel   upto 2*maxchannel:   frequency
     * 2*maxchannel+1 upto 3*maxchannel:   Peak Values with DC
     */

    int expectedChannelCount=m_rangeMeasChannelList.length();
    int deliveredChannelCount=(values->length()-1)/3;
    int rmsOffset=deliveredChannelCount;
    int frequencyPos=2*deliveredChannelCount;
    int peakWithDcOffset=deliveredChannelCount+1;
    // prevent potential out of range access
    int iterationLength=qMin(expectedChannelCount,deliveredChannelCount);
    for(int i=0; i<iterationLength;++i){
        m_rangeMeasChannelList.at(i)->setPeakValue(values->at(i));
        m_rangeMeasChannelList.at(i)->setRmsValue(values->at(rmsOffset+i));
        m_rangeMeasChannelList.at(i)->setSignalFrequency(values->at(frequencyPos));
        m_rangeMeasChannelList.at(i)->setPeakValueWithDc(values->at(peakWithDcOffset+i));
    }
}

}
