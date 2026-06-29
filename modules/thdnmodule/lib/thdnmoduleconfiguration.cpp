#include "thdnmoduleconfiguration.h"

namespace THDNMODULE
{

cThdnModuleConfiguration::cThdnModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setMeasureInterval,
    setMovingwindowBool,
    setMovingwindowTime,
    setThdnSourceEntity
};

void cThdnModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["thdnmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["thdnmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;
    m_ConfigXMLMap["thdnmodconfpar:configuration:thdr_shadow_of"] = setThdnSourceEntity;

    m_ConfigXMLMap["thdnmodconfpar:parameter:interval"] = setMeasureInterval;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cThdnModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cThdnModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cThdnModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramInterval = &m_configData.m_fMeasInterval;
    m_pXMLReader->setValue(paramInterval->m_sKey, QString("%1").arg(paramInterval->m_fValue));
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cThdnModuleConfigData *cThdnModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cThdnModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setMovingwindowBool:
            m_configData.m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_configData.m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setThdnSourceEntity:
            m_configData.m_thdrSourceEntity = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setMeasureInterval:
            m_configData.m_fMeasInterval.m_sKey = key;
            m_configData.m_fMeasInterval.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}

void cThdnModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

