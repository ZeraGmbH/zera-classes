#include "adjustmentmoduleconfiguration.h"
#include "adjustmentmoduleconfigdata.h"
#include <xmlconfigreader.h>

cAdjustmentModuleConfiguration::cAdjustmentModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cAdjustmentModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cAdjustmentModuleConfiguration::completeConfiguration);
}


cAdjustmentModuleConfiguration::~cAdjustmentModuleConfiguration()
{
    delete m_pAdjustmentModulConfigData;
}

enum moduleconfigstate
{
    setAngleReferenceEntity,
    setAngleReferenceComponent,
    setFrequencyReferenceEntity,
    setFrequencyReferenceComponent,
    setChannelCount,

    setChn1Chn = 16, // we leave some place for additional base cmds and commands for up 32 channels

    setChn1AmplInfoAvail = setChn1Chn +32,
    setChn1AmplInfoEntity = setChn1AmplInfoAvail +32,
    setChn1AmplInfoComponent = setChn1AmplInfoEntity + 32,

    setChn1PhaseInfoAvail = setChn1AmplInfoComponent + 32,
    setChn1PhaseInfoEntity = setChn1PhaseInfoAvail + 32,
    setChn1PhaseInfoComponent = setChn1PhaseInfoEntity + 32,

    setChn1OffsInfoAvail = setChn1PhaseInfoComponent + 32,
    setChn1OffsInfoEntity = setChn1OffsInfoAvail + 32,
    setChn1OffsInfoComponent = setChn1OffsInfoEntity + 32,

    setChn1RangeInfoEntity = setChn1OffsInfoComponent + 32,
    setChn1RangeInfoComponent = setChn1RangeInfoEntity + 32,
    setChn1RangeInfoRangesNotAdjustable = setChn1RangeInfoComponent + 32,
};

void cAdjustmentModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pAdjustmentModulConfigData) delete m_pAdjustmentModulConfigData;
    m_pAdjustmentModulConfigData = new cAdjustmentModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:anglereference:entity"] = setAngleReferenceEntity;
    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:anglereference:component"] = setAngleReferenceComponent;
    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:frequencyreference:entity"] = setFrequencyReferenceEntity;
    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:frequencyreference:component"] = setFrequencyReferenceComponent;

    m_ConfigXMLMap["adjmodconfpar:configuration:adjustment:channel:n"] = setChannelCount;
    // rest of hash table is initialized dynamically depending on channel count

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cAdjustmentModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cAdjustmentModuleConfigData *cAdjustmentModuleConfiguration::getConfigurationData()
{
    return m_pAdjustmentModulConfigData;
}


void cAdjustmentModuleConfiguration::configXMLInfo(const QString &key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
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

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:rangeinfo:entity").arg(i+1)] = setChn1RangeInfoEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:rangeinfo:component").arg(i+1)] = setChn1RangeInfoComponent + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:rangeinfo:ranges_not_adjustable").arg(i+1)] = setChn1RangeInfoRangesNotAdjustable + i;

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
                cmd -= setChn1AmplInfoAvail;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rmsAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1AmplInfoEntity) && (cmd < setChn1AmplInfoEntity + 32)) {
                cmd -= setChn1AmplInfoEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rmsAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1AmplInfoComponent) && (cmd < setChn1AmplInfoComponent + 32)) {
                cmd -= setChn1AmplInfoComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rmsAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }
            else if ((cmd >= setChn1PhaseInfoAvail) && (cmd < setChn1PhaseInfoAvail + 32)) {
                cmd -= setChn1PhaseInfoAvail;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->phaseAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1PhaseInfoEntity) && (cmd < setChn1PhaseInfoEntity + 32)) {
                cmd -= setChn1PhaseInfoEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->phaseAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1PhaseInfoComponent) && (cmd < setChn1PhaseInfoComponent + 32)) {
                cmd -= setChn1PhaseInfoComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->phaseAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }

            else if ((cmd >= setChn1OffsInfoAvail) && (cmd < setChn1OffsInfoAvail + 32)) {
                cmd -= setChn1OffsInfoAvail;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->dcAdjInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1OffsInfoEntity) && (cmd < setChn1OffsInfoEntity + 32)) {
                cmd -= setChn1OffsInfoEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->dcAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1OffsInfoComponent) && (cmd < setChn1OffsInfoComponent + 32)) {
                cmd -= setChn1OffsInfoComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->dcAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }
            else if ((cmd >= setChn1RangeInfoEntity) && (cmd < setChn1RangeInfoEntity + 32)) {
                cmd -= setChn1RangeInfoEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeAdjInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1RangeInfoComponent) && (cmd < setChn1RangeInfoComponent + 32)) {
                cmd -= setChn1RangeInfoComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeAdjInfo.m_sComponent = m_pXMLReader->getValue(key);
            }
            else if ((cmd >= setChn1RangeInfoRangesNotAdjustable) && (cmd < setChn1RangeInfoRangesNotAdjustable + 32)) {
                cmd -= setChn1RangeInfoRangesNotAdjustable;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeAdjInfo.m_rangesNotAdjustable = m_pXMLReader->getValue(key).split(",", Qt::SkipEmptyParts);
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
}

