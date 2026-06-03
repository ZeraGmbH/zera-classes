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

    setChn1AcRmsValueAvail = setChn1Chn +32,
    setChn1AcRmsValueEntity = setChn1AcRmsValueAvail +32,
    setChn1AcRmsValueComponent = setChn1AcRmsValueEntity + 32,

    setChn1AcPhaseValueAvail = setChn1AcRmsValueComponent + 32,
    setChn1AcPhaseValueEntity = setChn1AcPhaseValueAvail + 32,
    setChn1AcPhaseValueComponent = setChn1AcPhaseValueEntity + 32,

    setChn1DcValueAvail = setChn1AcPhaseValueComponent + 32,
    setChn1DcValueEntity = setChn1DcValueAvail + 32,
    setChn1DcValueComponent = setChn1DcValueEntity + 32,

    setChn1RangeInfoEntity = setChn1DcValueComponent + 32,
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

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:ac_rms_value:avail").arg(i+1)] = setChn1AcRmsValueAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:ac_rms_value:entity").arg(i+1)] = setChn1AcRmsValueEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:ac_rms_value:component").arg(i+1)] = setChn1AcRmsValueComponent + i;

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:ac_phase_value:avail").arg(i+1)] = setChn1AcPhaseValueAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:ac_phase_value:entity").arg(i+1)] = setChn1AcPhaseValueEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:ac_phase_value:component").arg(i+1)] = setChn1AcPhaseValueComponent + i;

                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:dc_values:avail").arg(i+1)] = setChn1DcValueAvail + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:dc_values:entity").arg(i+1)] = setChn1DcValueEntity + i;
                m_ConfigXMLMap[QString("adjmodconfpar:configuration:adjustment:channel:chn%1:dc_values:component").arg(i+1)] = setChn1DcValueComponent + i;

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

            else if ((cmd >= setChn1AcRmsValueAvail) && (cmd < setChn1AcRmsValueAvail + 32)) {
                cmd -= setChn1AcRmsValueAvail;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->acRmsValueInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1AcRmsValueEntity) && (cmd < setChn1AcRmsValueEntity + 32)) {
                cmd -= setChn1AcRmsValueEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->acRmsValueInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1AcRmsValueComponent) && (cmd < setChn1AcRmsValueComponent + 32)) {
                cmd -= setChn1AcRmsValueComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->acRmsValueInfo.m_sComponent = m_pXMLReader->getValue(key);
            }
            else if ((cmd >= setChn1AcPhaseValueAvail) && (cmd < setChn1AcPhaseValueAvail + 32)) {
                cmd -= setChn1AcPhaseValueAvail;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->acPhaseValueInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1AcPhaseValueEntity) && (cmd < setChn1AcPhaseValueEntity + 32)) {
                cmd -= setChn1AcPhaseValueEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->acPhaseValueInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1AcPhaseValueComponent) && (cmd < setChn1AcPhaseValueComponent + 32)) {
                cmd -= setChn1AcPhaseValueComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->acPhaseValueInfo.m_sComponent = m_pXMLReader->getValue(key);
            }

            else if ((cmd >= setChn1DcValueAvail) && (cmd < setChn1DcValueAvail + 32)) {
                cmd -= setChn1DcValueAvail;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->dcValueInfo.m_bAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            }
            else if ((cmd >= setChn1DcValueEntity) && (cmd < setChn1DcValueEntity + 32)) {
                cmd -= setChn1DcValueEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->dcValueInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1DcValueComponent) && (cmd < setChn1DcValueComponent + 32)) {
                cmd -= setChn1DcValueComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->dcValueInfo.m_sComponent = m_pXMLReader->getValue(key);
            }
            else if ((cmd >= setChn1RangeInfoEntity) && (cmd < setChn1RangeInfoEntity + 32)) {
                cmd -= setChn1RangeInfoEntity;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeInfo.m_nEntity = m_pXMLReader->getValue(key).toInt(&ok);
            }
            else if ((cmd >= setChn1RangeInfoComponent) && (cmd < setChn1RangeInfoComponent + 32)) {
                cmd -= setChn1RangeInfoComponent;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeInfo.m_sComponent = m_pXMLReader->getValue(key);
            }
            else if ((cmd >= setChn1RangeInfoRangesNotAdjustable) && (cmd < setChn1RangeInfoRangesNotAdjustable + 32)) {
                cmd -= setChn1RangeInfoRangesNotAdjustable;
                QString chn = m_pAdjustmentModulConfigData->m_AdjChannelList.at(cmd);
                cAdjChannelInfo* adjChannelInfo = m_pAdjustmentModulConfigData->m_AdjChannelInfoHash[chn];
                adjChannelInfo->rangeInfo.m_rangesNotAdjustable = m_pXMLReader->getValue(key).split(",", Qt::SkipEmptyParts);
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

