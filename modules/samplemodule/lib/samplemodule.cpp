#include "samplemodule.h"
#include "samplemoduleconfiguration.h"

namespace SAMPLEMODULE
{

cSampleModule::cSampleModule(ModuleFactoryParam moduleParam) :
    cBaseMeasModule(moduleParam, std::make_shared<cSampleModuleConfiguration>())
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module is responsible for pll range setting\n, pll channel selection and automatic");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cPllMeasChannel* cSampleModule::getPllMeasChannel(const QString &name)
{
    cPllMeasChannel* p_pllmc = 0;
    for (int i = 0; i < m_pllMeasChannelList.count(); i++) {
        p_pllmc =  m_pllMeasChannelList.at(i);
        if ((p_pllmc->getMName()) == name)
            return p_pllmc;
    }
    return p_pllmc;
}

void cSampleModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    cBaseMeasModule::setupModule();

    cSampleModuleConfigData *pConfData;
    pConfData = qobject_cast<cSampleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // first we build a list of our pll meas channels, that hold informations for other activists
    for (int i = 0; i < pConfData->m_ObsermaticConfPar.m_npllChannelCount; i ++) {
        const QString channelMName = pConfData->m_ObsermaticConfPar.m_pllChannelList.at(i);
        cPllMeasChannel* pllchn = new cPllMeasChannel(getSharedChannelRangeObserver()->getChannel(channelMName),
                                                      getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                      getNetworkConfig()->m_tcpNetworkFactory,
                                                      getVeinModuleName());
        m_pllMeasChannelList.append(pllchn);
        m_ModuleActivistList.append(pllchn);
        connect(pllchn, &cPllMeasChannel::activated, this, &cSampleModule::activationContinue);
        connect(pllchn, &cPllMeasChannel::deactivated, this, &cSampleModule::deactivationContinue);
    }

    // we need some program that does the pll handling (observation, automatic, setting)
    m_pPllObsermatic = new cPllObsermatic(this, *pConfData);
    m_ModuleActivistList.append(m_pPllObsermatic);
    connect(m_pPllObsermatic, &cPllObsermatic::activated, this, &cSampleModule::activationContinue);
    connect(m_pPllObsermatic, &cPllObsermatic::deactivated, this, &cSampleModule::deactivationContinue);

    // at last we need some program that does the measuring on dsp
    m_pMeasProgram = new cSampleModuleMeasProgram(this, m_pConfiguration);
    m_ModuleActivistList.append(m_pMeasProgram);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::activated, this, &cSampleModule::activationContinue);
    connect(m_pMeasProgram, &cSampleModuleMeasProgram::deactivated, this, &cSampleModule::deactivationContinue);

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSampleModule::startMeas()
{
    m_pMeasProgram->start();
}

void cSampleModule::stopMeas()
{
    m_pMeasProgram->stop();
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

    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);
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

}
