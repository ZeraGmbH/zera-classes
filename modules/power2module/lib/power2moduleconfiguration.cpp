#include "power2moduleconfiguration.h"
#include "foutinfo.h"

namespace POWER2MODULE
{

cPower2ModuleConfiguration::cPower2ModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setMeasModeCount,
    setMeasSystem1,
    setMeasSystem2,
    setMeasSystem3,
    setIntegrationMode,
    setMovingwindowBool,
    setMovingwindowTime,
    setNominalFrequency,
    setFrequencyActualizationMode,
    setFrequencyOutputCount,

    setMeasuringMode,
    setMeasureIntervalTime,
    setMeasureIntervalPeriod,

    setMeasMode1 = 32, // we leave some place for additional cmds

    setfreqout1Name = setMeasMode1 + 32, // 32 measuring modes should be enough
    setfreqout2Name,
    setfreqout3Name,
    setfreqout4Name,

    setfreqout1Source = setfreqout1Name +8, // also some place for additional frequency outputs
    setfreqout2Source,
    setfreqout3Source,
    setfreqout4Source,

    setfreqout1Type = setfreqout1Source + 8,
    setfreqout2Type,
    setfreqout3Type,
    setfreqout4Type,

    setnext = setfreqout1Type + 8
};

void cPower2ModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cPower2ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cPower2ModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["pow2modconfpar:configuration:measure:modes:n"] = setMeasModeCount;

    m_ConfigXMLMap["pow2modconfpar:configuration:measure:system:pms1"] = setMeasSystem1;
    m_ConfigXMLMap["pow2modconfpar:configuration:measure:system:pms2"] = setMeasSystem2;
    m_ConfigXMLMap["pow2modconfpar:configuration:measure:system:pms3"] = setMeasSystem3;

    m_ConfigXMLMap["pow2modconfpar:configuration:measure:integrationmode"] = setIntegrationMode;
    m_ConfigXMLMap["pow2modconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["pow2modconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;

    m_ConfigXMLMap["pow2modconfpar:configuration:frequencyoutput:frequency"] = setNominalFrequency;
    m_ConfigXMLMap["pow2modconfpar:configuration:frequencyoutput:frequencyact"] = setFrequencyActualizationMode;
    m_ConfigXMLMap["pow2modconfpar:configuration:frequencyoutput:output:n"] = setFrequencyOutputCount;

    m_ConfigXMLMap["pow2modconfpar:parameter:measuringmode"] = setMeasuringMode;
    m_ConfigXMLMap["pow2modconfpar:parameter:interval:time"] = setMeasureIntervalTime;
    m_ConfigXMLMap["pow2modconfpar:parameter:interval:period"] = setMeasureIntervalPeriod;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cPower2ModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramIntervall = &m_configData.m_fMeasIntervalTime;
    m_pXMLReader->setValue(paramIntervall->m_sKey, QString("%1").arg(paramIntervall->m_fValue));

    const intParameter* paramPeriod = &m_configData.m_nMeasIntervalPeriod;
    m_pXMLReader->setValue(paramPeriod->m_sKey, QString("%1").arg(paramPeriod->m_nValue));

    const stringParameter* paramMeasMode = &m_configData.m_sMeasuringMode;
    m_pXMLReader->setValue(paramMeasMode->m_sKey, paramMeasMode->m_sValue);

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cPower2ModuleConfigData *cPower2ModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cPower2ModuleConfiguration::addMeasSys(const QString &val)
{
    if(!val.isEmpty())
        m_configData.m_sMeasSystemList.append(val);
}

void cPower2ModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setMeasModeCount:
            m_configData.m_nMeasModeCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nMeasModeCount; i++)
                m_ConfigXMLMap[QString("pow2modconfpar:configuration:measure:modes:m%1").arg(i+1)] = setMeasMode1+i;
            break;
        case setMeasSystem1:
            Q_ASSERT(m_configData.m_sMeasSystemList.count() == 0);
            addMeasSys(m_pXMLReader->getValue(key));
            break;
        case setMeasSystem2:
            Q_ASSERT(m_configData.m_sMeasSystemList.count() == 1);
            addMeasSys(m_pXMLReader->getValue(key));
            break;
        case setMeasSystem3:
            Q_ASSERT(m_configData.m_sMeasSystemList.count() == 2);
            addMeasSys(m_pXMLReader->getValue(key));
            break;
        case setIntegrationMode:
            m_configData.m_sIntegrationMode = m_pXMLReader->getValue(key);
            break;
        case setMovingwindowBool:
            m_configData.m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_configData.m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setNominalFrequency:
            m_configData.m_nNominalFrequency = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setFrequencyActualizationMode:
            m_configData.m_sFreqActualizationMode = m_pXMLReader->getValue(key);
            break;
        case setFrequencyOutputCount:
            m_configData.m_nFreqOutputCount = m_pXMLReader->getValue(key).toInt(&ok);
            if (m_configData.m_nFreqOutputCount > 0) {
                for (int i = 0; i < m_configData.m_nFreqOutputCount; i++) {
                    m_ConfigXMLMap[QString("pow2modconfpar:configuration:frequencyoutput:output:fout%1:name").arg(i+1)] = setfreqout1Name+i;
                    m_ConfigXMLMap[QString("pow2modconfpar:configuration:frequencyoutput:output:fout%1:source").arg(i+1)] = setfreqout1Source+i;
                    m_ConfigXMLMap[QString("pow2modconfpar:configuration:frequencyoutput:output:fout%1:type").arg(i+1)] = setfreqout1Type+i;
                    freqoutconfiguration fconf;
                    m_configData.m_FreqOutputConfList.append(fconf);
                }
            }
            break;
        case setMeasuringMode:
            m_configData.m_sMeasuringMode.m_sKey = key;
            m_configData.m_sMeasuringMode.m_sValue = m_pXMLReader->getValue(key);
            break;
        case setMeasureIntervalTime:
            m_configData.m_fMeasIntervalTime.m_sKey = key;
            m_configData.m_fMeasIntervalTime.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIntervalPeriod:
            m_configData.m_nMeasIntervalPeriod.m_sKey = key;
            m_configData.m_nMeasIntervalPeriod.m_nValue = m_pXMLReader->getValue(key).toInt(&ok);
            break;

        default:
            // here we decode the dyn. generated cmd's
            if ((cmd >= setMeasMode1) && (cmd < setMeasMode1 + 32)) {
                cmd -= setMeasMode1;
                // it is command for setting measuring mode
                QString measMode = m_pXMLReader->getValue(key);
                m_configData.m_sMeasmodeList.append(measMode); // for configuration of our engine
            }
            else if ((cmd >= setfreqout1Name) && (cmd < setfreqout1Name + 8)) {
                cmd -= setfreqout1Name;
                freqoutconfiguration fconf;
                fconf = m_configData.m_FreqOutputConfList.at(cmd);
                fconf.m_sName = m_pXMLReader->getValue(key);
                m_configData.m_FreqOutputConfList.replace(cmd, fconf);
            }
            else if ((cmd >= setfreqout1Source) && (cmd < setfreqout1Source + 8)) {
                QString s;
                s = m_pXMLReader->getValue(key);
                cmd -= setfreqout1Source;
                freqoutconfiguration fconf;
                fconf = m_configData.m_FreqOutputConfList.at(cmd);
                if (s == "pms1")
                    fconf.m_nSource = 0;
                if (s == "pms2")
                    fconf.m_nSource = 1;
                if (s == "pms3")
                    fconf.m_nSource = 2;
                if (s == "pmss")
                    fconf.m_nSource = 3;

                m_configData.m_FreqOutputConfList.replace(cmd, fconf);
            }
            else if ((cmd >= setfreqout1Type) && (cmd < setfreqout1Type+ 8)) {
                QString s;
                cmd -= setfreqout1Type;
                s = m_pXMLReader->getValue(key);
                freqoutconfiguration fconf;
                fconf = m_configData.m_FreqOutputConfList.at(cmd);
                if (s == "+-")
                    fconf.m_nFoutMode = absPower;
                if (s == "+")
                    fconf.m_nFoutMode = posPower;
                if (s == "-")
                    fconf.m_nFoutMode = negPower;
                m_configData.m_FreqOutputConfList.replace(cmd, fconf);
            }
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void cPower2ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
