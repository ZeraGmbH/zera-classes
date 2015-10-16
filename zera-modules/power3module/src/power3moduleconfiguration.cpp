#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "power3moduleconfiguration.h"
#include "power3moduleconfigdata.h"
#include "socket.h"

namespace POWER3MODULE
{

cPower3ModuleConfiguration::cPower3ModuleConfiguration()
{
    m_pPower3ModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cPower3ModuleConfiguration::~cPower3ModuleConfiguration()
{
    if (m_pPower3ModulConfigData) delete m_pPower3ModulConfigData;
}


void cPower3ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pPower3ModulConfigData) delete m_pPower3ModulConfigData;
    m_pPower3ModulConfigData = new cPower3ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["pow3modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;

    m_ConfigXMLMap["pow3modconfpar:configuration:measure:input"] = setInputModule;
    m_ConfigXMLMap["pow3modconfpar:configuration:measure:system:n"] = setSystemCount;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cPower3ModuleConfiguration::exportConfiguration()
{
    // no parameters -> nothing to export
}


cPower3ModuleConfigData *cPower3ModuleConfiguration::getConfigurationData()
{
    return m_pPower3ModulConfigData;
}


void cPower3ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pPower3ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInputModule:
            m_pPower3ModulConfigData->m_sInputModule = m_pXMLReader->getValue(key);
            break;
        case setSystemCount:
        {
            powersystemconfiguration psc;
            psc.m_sInputU = "ACT_FFT1";
            psc.m_sInputI = "ACT_FFT4";

            m_pPower3ModulConfigData->m_nPowerSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pPower3ModulConfigData->m_nPowerSystemCount; i++)
            {
                m_ConfigXMLMap[QString("pow3modconfpar:configuration:measure:system:pms%1").arg(i+1)] = setMeasSystem1+i;
                m_pPower3ModulConfigData->m_powerSystemConfigList.append(psc);
            }
            break;
        }
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setMeasSystem1) && (cmd < setMeasSystem1 + 12))
            {
                cmd -= setMeasSystem1;
                // it is command for setting measuring mode
                QString measSystem = m_pXMLReader->getValue(key);
                QStringList measChannels = measSystem.split(",");
                powersystemconfiguration psc = m_pPower3ModulConfigData->m_powerSystemConfigList.at(cmd);
                psc.m_sInputU = measChannels.at(0);
                psc.m_sInputI = measChannels.at(1);
                m_pPower3ModulConfigData->m_powerSystemConfigList.replace(cmd, psc);
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cPower3ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

