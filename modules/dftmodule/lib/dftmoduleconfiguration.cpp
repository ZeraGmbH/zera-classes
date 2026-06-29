#include "dftmoduleconfiguration.h"

namespace DFTMODULE
{

enum moduleconfigstate
{
    setDftOrder,
    setValueCount,
    setrfieldChn1,
    setrfieldChn2,
    setrfieldChn3,
    setMeasureInterval,
    setRefChannel,
    setMovingwindowBool,
    setMovingwindowTime,
    setRefChannelOn,

    setValue1 = 20
};

cDftModuleConfiguration::cDftModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

void cDftModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cDftModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cDftModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["dftmodconfpar:configuration:measure:dftvalues:nr"] = setDftOrder;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:dftvalues:n"] = setValueCount;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:rfield:val1"] = setrfieldChn1;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:rfield:val2"] = setrfieldChn2;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:rfield:val3"] = setrfieldChn3;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;
    m_ConfigXMLMap["dftmodconfpar:configuration:measure:refchannelon"] = setRefChannelOn;

    m_ConfigXMLMap["dftmodconfpar:parameter:interval"] = setMeasureInterval;
    m_ConfigXMLMap["dftmodconfpar:parameter:refchannel"] = setRefChannel;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cDftModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramMeasInterval = &m_configData.m_fMeasInterval;
    m_pXMLReader->setValue(paramMeasInterval->m_sKey, QString("%1").arg(paramMeasInterval->m_fValue));

    const stringParameter* paramRefChannel = &m_configData.m_sRefChannel;
    m_pXMLReader->setValue(paramRefChannel->m_sKey, paramRefChannel->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cDftModuleConfigData *cDftModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cDftModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDftOrder:
            m_configData.m_nDftOrder = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setValueCount:
            m_configData.m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nValueCount; i++)
                m_ConfigXMLMap[QString("dftmodconfpar:configuration:measure:dftvalues:val%1").arg(i+1)] = setValue1+i;
            break;
        case setrfieldChn1:
        case setrfieldChn2:
        case setrfieldChn3:
            m_configData.m_rfieldChannelList.append(m_pXMLReader->getValue(key));
            break;
        case setMovingwindowBool:
            m_configData.m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_configData.m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureInterval:
            m_configData.m_fMeasInterval.m_sKey = key;
            m_configData.m_fMeasInterval.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setRefChannel:
            m_configData.m_sRefChannel.m_sKey = key;
            m_configData.m_sRefChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefChannelOn:
            m_configData.m_bRefChannelOn = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;

        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                //cmd -= setValue1;
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


void cDftModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

