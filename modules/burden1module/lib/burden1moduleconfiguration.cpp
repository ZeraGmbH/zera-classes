#include "burden1moduleconfiguration.h"
#include <QString>
#include <xmlconfigreader.h>

static const char* defaultXSDFile = "://burden1module.xsd";

namespace BURDEN1MODULE
{

cBurden1ModuleConfiguration::cBurden1ModuleConfiguration()
{
    m_pBurden1ModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cBurden1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cBurden1ModuleConfiguration::completeConfiguration);
}

cBurden1ModuleConfiguration::~cBurden1ModuleConfiguration()
{
    if (m_pBurden1ModulConfigData) delete m_pBurden1ModulConfigData;
}

void cBurden1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pBurden1ModulConfigData) delete m_pBurden1ModulConfigData;
    m_pBurden1ModulConfigData = new cBurden1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["brd1modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;

    m_ConfigXMLMap["brd1modconfpar:configuration:measure:inputentity"] = setInputModule;
    m_ConfigXMLMap["brd1modconfpar:configuration:measure:system:n"] = setSystemCount;
    m_ConfigXMLMap["brd1modconfpar:configuration:measure:system:unit"] = setSystemUnit;

    m_ConfigXMLMap["brd1modconfpar:parameter:nominalrange"] = setNominalRange;
    m_ConfigXMLMap["brd1modconfpar:parameter:nominalrangefactor"] = setNominalRangeFactor;
    m_ConfigXMLMap["brd1modconfpar:parameter:nominalburden"] = setNominalBurden;
    m_ConfigXMLMap["brd1modconfpar:parameter:wirelength"] = setWireLength;
    m_ConfigXMLMap["brd1modconfpar:parameter:wirecrosssection"] = setWireCrosssection;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}

QByteArray cBurden1ModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cBurden1ModuleConfigData *cBurden1ModuleConfiguration::getConfigurationData()
{
    return m_pBurden1ModulConfigData;
}


void cBurden1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pBurden1ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInputModule:
            m_pBurden1ModulConfigData->m_nModuleId = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSystemCount:
        {
            burdensystemconfiguration bsc;
            bsc.m_sInputVoltageVector = "ACT_DFTPN1";
            bsc.m_sInputCurrentVector = "ACT_DFTPN2";

            m_pBurden1ModulConfigData->m_nBurdenSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pBurden1ModulConfigData->m_nBurdenSystemCount; i++)
            {
                m_ConfigXMLMap[QString("brd1modconfpar:configuration:measure:system:bds%1").arg(i+1)] = setMeasSystem1+i;
                m_pBurden1ModulConfigData->m_BurdenSystemConfigList.append(bsc);
                m_pBurden1ModulConfigData->m_BurdenChannelList.append("");
            }
            break;
        }
        case setSystemUnit:
            m_pBurden1ModulConfigData->m_Unit = m_pXMLReader->getValue(key);
            break;
        case setNominalRange:
            m_pBurden1ModulConfigData->nominalRange.m_sKey = key;
            m_pBurden1ModulConfigData->nominalRange.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setNominalRangeFactor:
            m_pBurden1ModulConfigData->nominalRangeFactor.m_sKey = key;
            m_pBurden1ModulConfigData->nominalRangeFactor.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setNominalBurden:
            m_pBurden1ModulConfigData->nominalBurden.m_sKey = key;
            m_pBurden1ModulConfigData->nominalBurden.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setWireLength:
            m_pBurden1ModulConfigData->wireLength.m_sKey = key;
            m_pBurden1ModulConfigData->wireLength.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setWireCrosssection:
            m_pBurden1ModulConfigData->wireCrosssection.m_sKey = key;
            m_pBurden1ModulConfigData->wireCrosssection.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setMeasSystem1) && (cmd < setMeasSystem1 + 12))
            {
                cmd -= setMeasSystem1;
                // it is command for setting measuring mode
                QString brdSystem = m_pXMLReader->getValue(key);
                QStringList brdChannels = brdSystem.split(",");
                burdensystemconfiguration bsc = m_pBurden1ModulConfigData->m_BurdenSystemConfigList.at(cmd);
                bsc.m_sInputVoltageVector = brdChannels.at(0);
                bsc.m_sInputCurrentVector = brdChannels.at(1);

                m_pBurden1ModulConfigData->m_BurdenSystemConfigList.replace(cmd, bsc);
                m_pBurden1ModulConfigData->m_BurdenChannelList.replace(cmd, brdChannels.at(2));
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}

void cBurden1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

