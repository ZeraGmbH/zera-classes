#include "samplemoduleconfiguration.h"

namespace SAMPLEMODULE
{

cSampleModuleConfiguration::cSampleModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setPllFixedBool,
    setPllAutomatic,
    setPllChannelCount, // the number of sense channels the pll can be set to

    setPllChannel1 = 16, // max. 32 channels
    setPllChannel = 48, // the pll setting we start with
};

void cSampleModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["samplemodconfpar:configuration:sample:pllfixed"] = setPllFixedBool;

    m_ConfigXMLMap["samplemodconfpar:configuration:sample:pllchannel:n"] = setPllChannelCount;
    // delegates for setting channels itself are generated dynamically

    m_ConfigXMLMap["samplemodconfpar:parameter:sample:pllauto"] = setPllAutomatic;
    m_ConfigXMLMap["samplemodconfpar:parameter:sample:pllchannel"] = setPllChannel;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSampleModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSampleModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cSampleModuleConfiguration::exportConfiguration() const
{
    const boolParameter* paramPllAuto = &m_configData.m_ObsermaticConfPar.m_npllAutoAct;
    m_pXMLReader->setValue(paramPllAuto->m_sKey, QString("%1").arg(paramPllAuto->m_nActive));

    const stringParameter* paramPllChannel = &m_configData.m_ObsermaticConfPar.m_pllSystemChannel;
    m_pXMLReader->setValue(paramPllChannel->m_sKey, paramPllChannel->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cSampleModuleConfigData *cSampleModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cSampleModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setPllChannelCount:
        {
            m_configData.m_ObsermaticConfPar.m_npllChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for pll channel configuration
            QString channel;
            for (int i = 0; i < m_configData.m_ObsermaticConfPar.m_npllChannelCount; i++)
            {
                m_ConfigXMLMap[QString("samplemodconfpar:configuration:sample:pllchannel:ch%1").arg(i+1)] = setPllChannel1+i;
                m_configData.m_ObsermaticConfPar.m_pllChannelList.append(channel);
            }
            break;
        }
        case setPllAutomatic:
            m_configData.m_ObsermaticConfPar.m_npllAutoAct.m_sKey = key;
            m_configData.m_ObsermaticConfPar.m_npllAutoAct.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPllFixedBool:
            m_configData.m_ObsermaticConfPar.m_bpllFixed = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setPllChannel:
            m_configData.m_ObsermaticConfPar.m_pllSystemChannel.m_sKey = key;
            m_configData.m_ObsermaticConfPar.m_pllSystemChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        default:
            if ((cmd >= setPllChannel1) && (cmd < setPllChannel1 + 32))
            {
                cmd -= setPllChannel1;
                m_configData.m_ObsermaticConfPar.m_pllChannelList.replace(cmd, m_pXMLReader->getValue(key));
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}

void cSampleModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

