#include "burden1moduleconfiguration.h"

namespace BURDEN1MODULE
{

cBurden1ModuleConfiguration::cBurden1ModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setInputModule,
    setSystemCount,
    setSystemUnit,

    setNominalRange,
    setNominalRangeFactor,
    setNominalBurden,
    setWireLength,
    setWireCrosssection,

    setMeasSystem1 = 16, // we leave some place for additional cmds

    setnext = setMeasSystem1 + 16
};

void cBurden1ModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["brd1modconfpar:configuration:measure:inputentity"] = setInputModule;
    m_ConfigXMLMap["brd1modconfpar:configuration:measure:system:n"] = setSystemCount;
    m_ConfigXMLMap["brd1modconfpar:configuration:measure:system:unit"] = setSystemUnit;

    m_ConfigXMLMap["brd1modconfpar:parameter:nominalrange"] = setNominalRange;
    m_ConfigXMLMap["brd1modconfpar:parameter:nominalrangefactor"] = setNominalRangeFactor;
    m_ConfigXMLMap["brd1modconfpar:parameter:nominalburden"] = setNominalBurden;
    m_ConfigXMLMap["brd1modconfpar:parameter:wirelength"] = setWireLength;
    m_ConfigXMLMap["brd1modconfpar:parameter:wirecrosssection"] = setWireCrosssection;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cBurden1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cBurden1ModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cBurden1ModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramNominalRange = &m_configData.nominalRange;
    m_pXMLReader->setValue(paramNominalRange->m_sKey, QString("%1").arg(paramNominalRange->m_fValue));

    const stringParameter *paramNminalRangeFactor = &m_configData.nominalRangeFactor;
    m_pXMLReader->setValue(paramNminalRangeFactor->m_sKey, paramNminalRangeFactor->m_sPar);

    const doubleParameter* paramNominalBurden = &m_configData.nominalBurden;
    m_pXMLReader->setValue(paramNominalBurden->m_sKey, QString("%1").arg(paramNominalBurden->m_fValue));

    const doubleParameter* paramWireLength = &m_configData.wireLength;
    m_pXMLReader->setValue(paramWireLength->m_sKey, QString("%1").arg(paramWireLength->m_fValue));

    const doubleParameter*paramWireCrosssection = &m_configData.wireCrosssection;
    m_pXMLReader->setValue(paramWireCrosssection->m_sKey, QString("%1").arg(paramWireCrosssection->m_fValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cBurden1ModuleConfigData *cBurden1ModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cBurden1ModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setInputModule:
            m_configData.m_nModuleId = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSystemCount:
        {
            burdensystemconfiguration bsc;
            bsc.m_sInputVoltageVector = "ACT_DFTPN1";
            bsc.m_sInputCurrentVector = "ACT_DFTPN2";

            m_configData.m_nBurdenSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nBurdenSystemCount; i++)
            {
                m_ConfigXMLMap[QString("brd1modconfpar:configuration:measure:system:bds%1").arg(i+1)] = setMeasSystem1+i;
                m_configData.m_BurdenSystemConfigList.append(bsc);
                m_configData.m_BurdenChannelList.append("");
            }
            break;
        }
        case setSystemUnit:
            m_configData.m_Unit = m_pXMLReader->getValue(key);
            break;
        case setNominalRange:
            m_configData.nominalRange.m_sKey = key;
            m_configData.nominalRange.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setNominalRangeFactor:
            m_configData.nominalRangeFactor.m_sKey = key;
            m_configData.nominalRangeFactor.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setNominalBurden:
            m_configData.nominalBurden.m_sKey = key;
            m_configData.nominalBurden.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setWireLength:
            m_configData.wireLength.m_sKey = key;
            m_configData.wireLength.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setWireCrosssection:
            m_configData.wireCrosssection.m_sKey = key;
            m_configData.wireCrosssection.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setMeasSystem1) && (cmd < setMeasSystem1 + 12))
            {
                cmd -= setMeasSystem1;
                // it is command for setting measuring mode
                QString brdSystem = m_pXMLReader->getValue(key);
                QStringList brdChannels = brdSystem.split(",");
                burdensystemconfiguration bsc = m_configData.m_BurdenSystemConfigList.at(cmd);
                bsc.m_sInputVoltageVector = brdChannels.at(0);
                bsc.m_sInputCurrentVector = brdChannels.at(1);

                m_configData.m_BurdenSystemConfigList.replace(cmd, bsc);
                m_configData.m_BurdenChannelList.replace(cmd, brdChannels.at(2));
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
}

}

