#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "samplemoduleconfiguration.h"
#include "samplemoduleconfigdata.h"
#include "networkconnectioninfo.h"

namespace SAMPLEMODULE
{
static const char* defaultXSDFile = "://samplemodule.xsd";

cSampleModuleConfiguration::cSampleModuleConfiguration()
{
    m_pSampleModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSampleModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSampleModuleConfiguration::completeConfiguration);
}


cSampleModuleConfiguration::~cSampleModuleConfiguration()
{
    if (m_pSampleModulConfigData) delete m_pSampleModulConfigData;
}


void cSampleModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSampleModulConfigData) delete m_pSampleModulConfigData;
    m_pSampleModulConfigData = new cSampleModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["samplemodconfpar:configuration:sample:system"] = setSampleSystem;
    m_ConfigXMLMap["samplemodconfpar:configuration:sample:withpllautomatic"] = setPllAutomaticBool;
    m_ConfigXMLMap["samplemodconfpar:configuration:sample:pllfixed"] = setPllFixedBool;

    m_ConfigXMLMap["samplemodconfpar:configuration:sample:pllchannel:n"] = setPllChannelCount;
    // delegates for setting channels itself are generated dynamically

    m_ConfigXMLMap["samplemodconfpar:configuration:measure:interval"] = setMeasureInterval;

    m_ConfigXMLMap["samplemodconfpar:parameter:sample:pllauto"] = setPllAutomatic;
    m_ConfigXMLMap["samplemodconfpar:parameter:sample:pllchannel"] = setPllChannel;
    m_ConfigXMLMap["samplemodconfpar:parameter:sample:range"] = setSampleSystemRange;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSampleModuleConfiguration::exportConfiguration()
{
    boolParameter* bPar;
    bPar = &m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllAutoAct;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    stringParameter* sPar;
    sPar = &m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllSystemChannel;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);
    sPar = &m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllRange;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSampleModuleConfigData *cSampleModuleConfiguration::getConfigurationData()
{
    return m_pSampleModulConfigData;
}


void cSampleModuleConfiguration::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setSampleSystem:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_sSampleSystem = m_pXMLReader->getValue(key);
            break;
        case setPllChannelCount:
        {
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for pll channel configuration
            QString channel;
            for (int i = 0; i < m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllChannelCount; i++)
            {
                m_ConfigXMLMap[QString("samplemodconfpar:configuration:sample:pllchannel:ch%1").arg(i+1)] = setPllChannel1+i;
                m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannelList.append(channel);
            }
            break;
        }
        case setPllAutomatic:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllAutoAct.m_sKey = key;
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_npllAutoAct.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPllAutomaticBool:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_bpllAuto = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setPllFixedBool:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_bpllFixed = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setPllChannel:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllSystemChannel.m_sKey = key;
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllSystemChannel.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setSampleSystemRange:
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllRange.m_sKey = key;
            m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllRange.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setMeasureInterval:
            m_pSampleModulConfigData->m_fMeasInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setPllChannel1) && (cmd < setPllChannel1 + 32))
            {
                cmd -= setPllChannel1;
                m_pSampleModulConfigData->m_ObsermaticConfPar.m_pllChannelList.replace(cmd, m_pXMLReader->getValue(key));
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cSampleModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

