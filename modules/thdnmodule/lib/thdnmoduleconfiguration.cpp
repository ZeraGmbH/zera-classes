#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "thdnmoduleconfiguration.h"
#include "thdnmoduleconfigdata.h"
#include "networkconnectioninfo.h"

namespace THDNMODULE
{
static const char* defaultXSDFile = "://thdnmodule.xsd";

cThdnModuleConfiguration::cThdnModuleConfiguration()
{
    m_pThdnModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cThdnModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cThdnModuleConfiguration::completeConfiguration);
}


cThdnModuleConfiguration::~cThdnModuleConfiguration()
{
    if (m_pThdnModulConfigData) delete m_pThdnModulConfigData;
}


void cThdnModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pThdnModulConfigData) delete m_pThdnModulConfigData;
    m_pThdnModulConfigData = new cThdnModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["thdnmodconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["thdnmodconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;
    m_ConfigXMLMap["thdnmodconfpar:configuration:measure:thd"] = setTHDType;

    m_ConfigXMLMap["thdnmodconfpar:configuration:measure:values:n"] = setValueCount;

    m_ConfigXMLMap["thdnmodconfpar:parameter:interval"] = setMeasureInterval;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cThdnModuleConfiguration::exportConfiguration()
{
    doubleParameter* dPar;
    dPar = &m_pThdnModulConfigData->m_fMeasInterval;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cThdnModuleConfigData *cThdnModuleConfiguration::getConfigurationData()
{
    return m_pThdnModulConfigData;
}


void cThdnModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setMovingwindowBool:
            m_pThdnModulConfigData->m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_pThdnModulConfigData->m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setTHDType:
            m_pThdnModulConfigData->m_sTHDType = m_pXMLReader->getValue(key);
            break;
        case setValueCount:
            m_pThdnModulConfigData->m_nValueCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pThdnModulConfigData->m_nValueCount; i++)
                m_ConfigXMLMap[QString("thdnmodconfpar:configuration:measure:values:val%1").arg(i+1)] = setValue1+i;
            break;
        case setMeasureInterval:
            m_pThdnModulConfigData->m_fMeasInterval.m_sKey = key;
            m_pThdnModulConfigData->m_fMeasInterval.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setValue1) && (cmd < setValue1 + 32))
            {
                cmd -= setValue1;
                // it is command for setting value channel name
                QString valueChannel = m_pXMLReader->getValue(key);
                m_pThdnModulConfigData->m_valueChannelList.append(valueChannel); // for configuration of our engine
            }
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

