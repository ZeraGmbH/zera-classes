#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "adjustmentmoduleconfiguration.h"
#include "adjustmentmoduleconfigdata.h"
#include "socket.h"

static const char* defaultXSDFile = "://adjustmentmodule.xsd";

cAdjustmentModuleConfiguration::cAdjustmentModuleConfiguration()
{
    m_pAdjustmentModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cAdjustmentModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cAdjustmentModuleConfiguration::completeConfiguration);
}


cAdjustmentModuleConfiguration::~cAdjustmentModuleConfiguration()
{
    if (m_pAdjustmentModulConfigData) delete m_pAdjustmentModulConfigData;
}


void cAdjustmentModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    validateAndSetConfig(xmlString, defaultXSDFile);
}

void cAdjustmentModuleConfiguration::validateAndSetConfig(QByteArray xmlString, QString xsdFilename)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pAdjustmentModulConfigData) delete m_pAdjustmentModulConfigData;
    m_pAdjustmentModulConfigData = new cAdjustmentModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["adjmodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["adjmodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["adjmodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["adjmodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBIp;
    m_ConfigXMLMap["adjmodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBPort;

    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:anglereference:entity"] = setAngleReferenceEntity;
    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:anglereference:component"] = setAngleReferenceComponent;
    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:frequencyreference:entity"] = setFrequencyReferenceEntity;
    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:frequencyreference:component"] = setFrequencyReferenceComponent;

    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:n"] = setChannelCount;
    // rest of hash table is initialized dynamically depending on channel count

    if (m_pXMLReader->loadSchema(xsdFilename))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;

}


QByteArray cAdjustmentModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cAdjustmentModuleConfigData *cAdjustmentModuleConfiguration::getConfigurationData()
{
    return m_pAdjustmentModulConfigData;
}


void cAdjustmentModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pAdjustmentModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pAdjustmentModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pAdjustmentModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBIp:
            m_pAdjustmentModulConfigData->m_PCBSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBPort:
            m_pAdjustmentModulConfigData->m_PCBSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setAngleReferenceEntity:
            m_pAdjustmentModulConfigData->m_ReferenceAngle.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setAngleReferenceComponent:
            m_pAdjustmentModulConfigData->m_ReferenceAngle.m_sComponent = m_pXMLReader->getValue(key);
            break;
        case setFrequencyReferenceEntity:
            m_pAdjustmentModulConfigData->m_ReferenceFrequency.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setFrequencyReferenceComponent:
            m_pAdjustmentModulConfigData->m_ReferenceFrequency.m_sComponent = m_pXMLReader->getValue(key);
            break;

        case setChannelCount:
        {
            int nr;
            nr = m_pAdjustmentModulConfigData->m_nAdjustmentChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < nr; i++)
            {
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:chn").arg(i+1)] = setChn1Chn + i;

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:amplitudeinfo:avail").arg(i+1)] = setChn1AmplInfoAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:amplitudeinfo:entity").arg(i+1)] = setChn1AmplInfoEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:amplitudeinfo:component").arg(i+1)] = setChn1AmplInfoComponent + i;

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:phaseinfo:avail").arg(i+1)] = setChn1PhaseInfoAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:phaseinfo:entity").arg(i+1)] = setChn1PhaseInfoEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:phaseinfo:component").arg(i+1)] = setChn1PhaseInfoComponent + i;

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:offsetinfo:avail").arg(i+1)] = setChn1OffsInfoAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:offsetinfo:entity").arg(i+1)] = setChn1OffsInfoEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:offsetinfo:component").arg(i+1)] = setChn1OffsInfoComponent + i;

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:rangeinfo:avail").arg(i+1)] = setChn1RangeInfoAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:rangeinfo:entity").arg(i+1)] = setChn1RangeInfoEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:rangeinfo:component").arg(i+1)] = setChn1RangeInfoComponent + i;
            }

            break;
        }
        default:
            // here we decode the dyn. generated cmd's
            if ((cmd >= setChn1Chn) && (cmd < setChn1Chn + 32)) {
                cmd -= setChn1Chn;
                // it is command for setting measuring mode
                QString chn = m_pXMLReader->getValue(key);

                m_pAdjustmentModulConfigData->m_AdjChannelList.append(chn);
                m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn] = new cAdjChannelInfo();
            }

            else if ((cmd >= setChn1AmplInfoAvail) && (cmd < setChn1AmplInfoAvail + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1AmplInfoAvail;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->amplitudeAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1AmplInfoEntity) && (cmd < setChn1AmplInfoEntity + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1AmplInfoEntity;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->amplitudeAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1AmplInfoComponent) && (cmd < setChn1AmplInfoComponent + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1AmplInfoComponent;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->amplitudeAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }

            else if ((cmd >= setChn1PhaseInfoAvail) && (cmd < setChn1PhaseInfoAvail + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1PhaseInfoAvail;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->phaseAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1PhaseInfoEntity) && (cmd < setChn1PhaseInfoEntity + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1PhaseInfoEntity;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->phaseAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1PhaseInfoComponent) && (cmd < setChn1PhaseInfoComponent + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1PhaseInfoComponent;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->phaseAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }

            else if ((cmd >= setChn1OffsInfoAvail) && (cmd < setChn1OffsInfoAvail + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1OffsInfoAvail;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->offsetAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1OffsInfoEntity) && (cmd < setChn1OffsInfoEntity + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1OffsInfoEntity;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->offsetAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1OffsInfoComponent) && (cmd < setChn1OffsInfoComponent + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1OffsInfoComponent;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->offsetAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }

            else if ((cmd >= setChn1RangeInfoAvail) && (cmd < setChn1RangeInfoAvail + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1RangeInfoAvail;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1RangeInfoEntity) && (cmd < setChn1RangeInfoEntity + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1RangeInfoEntity;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1RangeInfoComponent) && (cmd < setChn1RangeInfoComponent + 32)) {
                QString chn;
                cAdjChannelInfo* adjChannelInfo;

                cmd -= setChn1RangeInfoComponent;
                chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }

        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cAdjustmentModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

