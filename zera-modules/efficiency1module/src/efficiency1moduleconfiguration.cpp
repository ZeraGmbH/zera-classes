#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "efficiency1moduleconfiguration.h"
#include "efficiency1moduleconfigdata.h"
#include "socket.h"

namespace EFFICIENCY1MODULE
{

cEfficiency1ModuleConfiguration::cEfficiency1ModuleConfiguration()
{
    m_pEfficiency1ModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cEfficiency1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cEfficiency1ModuleConfiguration::completeConfiguration);
}


cEfficiency1ModuleConfiguration::~cEfficiency1ModuleConfiguration()
{
    if (m_pEfficiency1ModulConfigData) delete m_pEfficiency1ModulConfigData;
}


void cEfficiency1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pEfficiency1ModulConfigData) delete m_pEfficiency1ModulConfigData;
    m_pEfficiency1ModulConfigData = new cEfficiency1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["efc1modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;

    m_ConfigXMLMap["efc1modconfpar:configuration:measure:powerinput:inputentity"] = setInputModule4InputPower;
    m_ConfigXMLMap["efc1modconfpar:configuration:measure:powerinput:system:n"] = setInputSystemCount;

    m_ConfigXMLMap["efc1modconfpar:configuration:measure:poweroutput:inputentity"] = setInputModule4OutputPower;
    m_ConfigXMLMap["efc1modconfpar:configuration:measure:poweroutput:system:n"] = setOutputSystemCount;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cEfficiency1ModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cEfficiency1ModuleConfigData *cEfficiency1ModuleConfiguration::getConfigurationData()
{
    return m_pEfficiency1ModulConfigData;
}


void cEfficiency1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pEfficiency1ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInputModule4InputPower:
            m_pEfficiency1ModulConfigData->m_PowerInputConfiguration.m_nModuleId = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInputModule4OutputPower:
            m_pEfficiency1ModulConfigData->m_PowerOutputConfiguration.m_nModuleId = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInputSystemCount:
        {
            int n;
            m_pEfficiency1ModulConfigData->m_PowerInputConfiguration.powerInputList.clear();

            n = m_pEfficiency1ModulConfigData->m_PowerInputConfiguration.m_nPowerSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < n; i++)
                m_ConfigXMLMap[QString("efc1modconfpar:configuration:measure:powerinput:system:pms%1").arg(i+1)] = setInputPowerMeasSystem1+i;

            break;
        }
        case setOutputSystemCount:
        {
            int n;
            m_pEfficiency1ModulConfigData->m_PowerOutputConfiguration.powerInputList.clear();

            n = m_pEfficiency1ModulConfigData->m_PowerOutputConfiguration.m_nPowerSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < n; i++)
                m_ConfigXMLMap[QString("efc1modconfpar:configuration:measure:poweroutput:system:pms%1").arg(i+1)] = setOutputPowerMeassystem1+i;

            break;
        }
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setInputPowerMeasSystem1) && (cmd < setInputPowerMeasSystem1 + 12))
            {
                cmd -= setInputPowerMeasSystem1;
                // it is command for setting another power input component
                QString measSystem = m_pXMLReader->getValue(key);
                m_pEfficiency1ModulConfigData->m_PowerInputConfiguration.powerInputList.append(measSystem);

            }
            else
                if ((cmd >= setOutputPowerMeassystem1) && (cmd < setOutputPowerMeassystem1 + 12))
                {
                    cmd -= setOutputPowerMeassystem1;
                    // it is command for setting another power input component
                    QString measSystem = m_pXMLReader->getValue(key);
                    m_pEfficiency1ModulConfigData->m_PowerOutputConfiguration.powerInputList.append(measSystem);
                }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cEfficiency1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

