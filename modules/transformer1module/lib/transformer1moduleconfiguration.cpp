#include "transformer1moduleconfiguration.h"

namespace TRANSFORMER1MODULE
{

cTransformer1ModuleConfiguration::cTransformer1ModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setInputModule,
    setSystemCount,
    setSystemUnit,

    setPrimClampPrim,
    setPrimClampSec,
    setSecClampPrim,
    setSecClampSec,
    setPrimDut,
    setSecDut,

    setMeasSystem1 = 16, // we leave some place for additional cmds

    setnext = setMeasSystem1 + 16
};

void cTransformer1ModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["trf1modconfpar:configuration:measure:inputentity"] = setInputModule;
    m_ConfigXMLMap["trf1modconfpar:configuration:measure:system:n"] = setSystemCount;
    m_ConfigXMLMap["trf1modconfpar:configuration:measure:system:unit"] = setSystemUnit;

    m_ConfigXMLMap["trf1modconfpar:parameter:primclampprim"] = setPrimClampPrim;
    m_ConfigXMLMap["trf1modconfpar:parameter:primclampsec"] = setPrimClampSec;
    m_ConfigXMLMap["trf1modconfpar:parameter:secclampprim"] = setSecClampPrim;
    m_ConfigXMLMap["trf1modconfpar:parameter:secclampsec"] = setSecClampSec;
    m_ConfigXMLMap["trf1modconfpar:parameter:primdut"] = setPrimDut;
    m_ConfigXMLMap["trf1modconfpar:parameter:secdut"] = setSecDut;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cTransformer1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cTransformer1ModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cTransformer1ModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramPrimClampPrim = &m_configData.primClampPrim;
    m_pXMLReader->setValue(paramPrimClampPrim->m_sKey, QString("%1").arg(paramPrimClampPrim->m_fValue));

    const doubleParameter* paramPimClampSec = &m_configData.primClampSec;
    m_pXMLReader->setValue(paramPimClampSec->m_sKey, QString("%1").arg(paramPimClampSec->m_fValue));

    const doubleParameter* paramSecClampPrim = &m_configData.secClampPrim;
    m_pXMLReader->setValue(paramSecClampPrim->m_sKey, QString("%1").arg(paramSecClampPrim->m_fValue));

    const doubleParameter* paramSecClampSec = &m_configData.secClampSec;
    m_pXMLReader->setValue(paramSecClampSec->m_sKey, QString("%1").arg(paramSecClampSec->m_fValue));

    const doubleParameter* paramDutPrim = &m_configData.dutPrim;
    m_pXMLReader->setValue(paramDutPrim->m_sKey, QString("%1").arg(paramDutPrim->m_fValue));

    const doubleParameter* paramDuSec = &m_configData.dutSec;
    m_pXMLReader->setValue(paramDuSec->m_sKey, QString("%1").arg(paramDuSec->m_fValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cTransformer1ModuleConfigData *cTransformer1ModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cTransformer1ModuleConfiguration::configXMLInfo(const QString &key)
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
            transformersystemconfiguration tsc;
            tsc.m_sInputPrimaryVector = "ACT_DFTPN1"; // some default
            tsc.m_sInputSecondaryVector = "ACT_DFTPN2";

            m_configData.m_nTransformerSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_nTransformerSystemCount; i++)
            {
                m_ConfigXMLMap[QString("trf1modconfpar:configuration:measure:system:trs%1").arg(i+1)] = setMeasSystem1+i;
                m_configData.m_transformerSystemConfigList.append(tsc);
                m_configData.m_TransformerChannelList.append("");
            }
            break;
        }
        case setSystemUnit:
            m_configData.m_clampUnit = m_pXMLReader->getValue(key);
            break;
        case setPrimClampPrim:
            m_configData.primClampPrim.m_sKey = key;
            m_configData.primClampPrim.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setPrimClampSec:
            m_configData.primClampSec.m_sKey = key;
            m_configData.primClampSec.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setSecClampPrim:
            m_configData.secClampPrim.m_sKey = key;
            m_configData.secClampPrim.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setSecClampSec:
            m_configData.secClampSec.m_sKey = key;
            m_configData.secClampSec.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setPrimDut:
            m_configData.dutPrim.m_sKey = key;
            m_configData.dutPrim.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setSecDut:
            m_configData.dutSec.m_sKey = key;
            m_configData.dutSec.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setMeasSystem1) && (cmd < setMeasSystem1 + 12))
            {
                cmd -= setMeasSystem1;
                // it is command for setting measuring mode
                QString trfSystem = m_pXMLReader->getValue(key);
                QStringList trfChannels = trfSystem.split(",");
                transformersystemconfiguration tsc = m_configData.m_transformerSystemConfigList.at(cmd);
                tsc.m_sInputPrimaryVector = trfChannels.at(0);
                tsc.m_sInputSecondaryVector = trfChannels.at(1);

                m_configData.m_transformerSystemConfigList.replace(cmd, tsc);
                m_configData.m_TransformerChannelList.replace(cmd, trfChannels.at(2));
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cTransformer1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

