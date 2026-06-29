#include "modemoduleconfiguration.h"

namespace MODEMODULE
{

cModeModuleConfiguration::cModeModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setMeasMode,
    setSamplingChnNr,
    setSamplingSigPeriod,
    setSamplingMeasPeriod
};

void cModeModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["modemodconfpar:configuration:pcb:mode"] = setMeasMode;
    m_ConfigXMLMap["modemodconfpar:configuration:dsp:sampling:chnnr"] = setSamplingChnNr;
    m_ConfigXMLMap["modemodconfpar:configuration:dsp:sampling:signalperiod"] = setSamplingSigPeriod;
    m_ConfigXMLMap["modemodconfpar:configuration:dsp:sampling:measureperiod"] = setSamplingMeasPeriod;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cModeModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cModeModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cModeModuleConfiguration::exportConfiguration() const
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cModeModuleConfigData *cModeModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cModeModuleConfiguration::configXMLInfo(const QString &key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setMeasMode:
            m_configData.m_sMode = m_pXMLReader->getValue(key);
            break;
        case setSamplingChnNr:
            m_configData.m_nChannelnr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSamplingSigPeriod:
            m_configData.m_nSignalPeriod = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSamplingMeasPeriod:
            m_configData.m_nMeasurePeriod = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }

        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}

void cModeModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
