#include "fftmoduleconfiguration.h"

namespace FFTMODULE
{
enum moduleconfigstate
{
    setFftorder,
    setValueCount,
    setRefChannel,
    setMeasureInterval,
    setMovingwindowBool,
    setMovingwindowTime,

    setValue1 = 20
};

cFftModuleConfiguration::cFftModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

void cFftModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cFftModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cFftModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["fftmodconfpar:configuration:measure:values:nr"] = setFftorder;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:values:n"] = setValueCount;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["fftmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;

    m_ConfigXMLMap["fftmodconfpar:parameter:refchannel"] = setRefChannel;
    m_ConfigXMLMap["fftmodconfpar:parameter:interval"] = setMeasureInterval;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cFftModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* dPar = &m_configData.m_fMeasInterval;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cFftModuleConfigData *cFftModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cFftModuleConfiguration::configXMLInfo(const QString &key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setFftorder:
            m_configData.m_nFftOrder = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_configData.m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nValueCount; i++)
                m_ConfigXMLMap[QString("fftmodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setMovingwindowBool:
            m_configData.m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_configData.m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setRefChannel:
            m_configData.m_RefChannel.m_sKey = key;
            m_configData.m_RefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setMeasureInterval:
            m_configData.m_fMeasInterval.m_sKey = key;
            m_configData.m_fMeasInterval.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_configData.m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cFftModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

