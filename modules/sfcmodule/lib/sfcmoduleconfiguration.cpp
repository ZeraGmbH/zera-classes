#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "sfcmoduleconfiguration.h"
#include "sfcmoduleconfigdata.h"

namespace SFCMODULE
{
cSfcModuleConfiguration::cSfcModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSfcModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSfcModuleConfiguration::completeConfiguration);
}

cSfcModuleConfiguration::~cSfcModuleConfiguration()
{
    if (m_pSfcModulConfigData)
        delete m_pSfcModulConfigData;
}

void cSfcModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pSfcModulConfigData)
        delete m_pSfcModulConfigData;
    m_pSfcModulConfigData = new cSfcModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash tasfc for xml configuration

    m_ConfigXMLMap["sfcmodconfpar:configuration:measure:dutinput:n"] = setDutInputCount;
    m_ConfigXMLMap["sfcmodconfpar:parameter:measure:dutinput"] = setDutInputPar;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cSfcModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cSfcModuleConfigData *cSfcModuleConfiguration::getConfigurationData()
{
    return m_pSfcModulConfigData;
}

void cSfcModuleConfiguration::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
    {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDutInputCount:
            m_pSfcModulConfigData->m_nDutInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSfcModulConfigData->m_nDutInpCount; i++) {
                m_ConfigXMLMap[QString("sfcmodconfpar:configuration:measure:dutinput:inp%1").arg(i+1)] = setDutInput1Name+i;
                m_pSfcModulConfigData->m_dutInpList.append(QString());
            }
            break;
        case setDutInputPar:
            m_pSfcModulConfigData->m_sDutInput.m_sKey = key;
            m_pSfcModulConfigData->m_sDutInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        default:
            if ((cmd >= setDutInput1Name) && (cmd < setDutInput1Name + 32)) {
                cmd -= setDutInput1Name;
                QString name = m_pXMLReader->getValue(key);
                m_pSfcModulConfigData->m_dutInpList.replace(cmd, name);
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
