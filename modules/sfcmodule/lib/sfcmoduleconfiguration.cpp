#include "sfcmoduleconfiguration.h"

namespace SFCMODULE
{

cSfcModuleConfiguration::cSfcModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setDutInputCount,

    setDutInputPar,

    setDutInput1Name = 32,
};

void cSfcModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["sfcmodconfpar:configuration:measure:dutinput:n"] = setDutInputCount;
    m_ConfigXMLMap["sfcmodconfpar:parameter:measure:dutinput"] = setDutInputPar;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSfcModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSfcModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cSfcModuleConfiguration::exportConfiguration() const
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cSfcModuleConfigData *cSfcModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cSfcModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDutInputCount:
            m_configData.m_nDutInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nDutInpCount; i++) {
                m_ConfigXMLMap[QString("sfcmodconfpar:configuration:measure:dutinput:inp%1").arg(i+1)] = setDutInput1Name+i;
                m_configData.m_dutInpList.append(QString());
            }
            break;
        case setDutInputPar:
            m_configData.m_sDutInput.m_sKey = key;
            m_configData.m_sDutInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        default:
            if ((cmd >= setDutInput1Name) && (cmd < setDutInput1Name + 32)) {
                cmd -= setDutInput1Name;
                QString name = m_pXMLReader->getValue(key);
                m_configData.m_dutInpList.replace(cmd, name);
            }
            break;
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void cSfcModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
