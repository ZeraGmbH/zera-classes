#include "recordermoduleconfiguration.h"
#include "recordermoduleconfigdata.h"

RecorderModuleConfiguration::RecorderModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &RecorderModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &RecorderModuleConfiguration::completeConfiguration);

}

RecorderModuleConfiguration::~RecorderModuleConfiguration()
{
    if(m_pRecorderModuleConfigData)
        delete m_pRecorderModuleConfigData;
}

void RecorderModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pRecorderModuleConfigData)
        delete m_pRecorderModuleConfigData;
    m_pRecorderModuleConfigData = new RecorderModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray RecorderModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

RecorderModuleConfigData *RecorderModuleConfiguration::getConfigurationData()
{
    return m_pRecorderModuleConfigData;
}

void RecorderModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;
    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void RecorderModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}
