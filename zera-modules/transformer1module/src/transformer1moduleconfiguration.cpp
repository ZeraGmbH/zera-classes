#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "transformer1moduleconfiguration.h"
#include "transformer1moduleconfigdata.h"
#include "socket.h"

namespace TRANSFORMER1MODULE
{

cTransformer1ModuleConfiguration::cTransformer1ModuleConfiguration()
{
    m_pTransformer1ModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cTransformer1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cTransformer1ModuleConfiguration::completeConfiguration);
}


cTransformer1ModuleConfiguration::~cTransformer1ModuleConfiguration()
{
    if (m_pTransformer1ModulConfigData) delete m_pTransformer1ModulConfigData;
}


void cTransformer1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pTransformer1ModulConfigData) delete m_pTransformer1ModulConfigData;
    m_pTransformer1ModulConfigData = new cTransformer1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["trf1modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;

    m_ConfigXMLMap["trf1modconfpar:configuration:measure:inputentity"] = setInputModule;
    m_ConfigXMLMap["trf1modconfpar:configuration:measure:system:n"] = setSystemCount;
    m_ConfigXMLMap["trf1modconfpar:configuration:measure:system:unit"] = setSystemUnit;

    m_ConfigXMLMap["trf1modconfpar:parameter:primclampprim"] = setPrimClampPrim;
    m_ConfigXMLMap["trf1modconfpar:parameter:primclampsec"] = setPrimClampSec;
    m_ConfigXMLMap["trf1modconfpar:parameter:secclampprim"] = setSecClampPrim;
    m_ConfigXMLMap["trf1modconfpar:parameter:secclampsec"] = setSecClampSec;
    m_ConfigXMLMap["trf1modconfpar:parameter:primdut"] = setPrimDut;
    m_ConfigXMLMap["trf1modconfpar:parameter:secdut"] = setSecDut;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cTransformer1ModuleConfiguration::exportConfiguration()
{
    doubleParameter* dPar;
    dPar = &m_pTransformer1ModulConfigData->primClampPrim;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    dPar = &m_pTransformer1ModulConfigData->primClampSec;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    dPar = &m_pTransformer1ModulConfigData->secClampPrim;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    dPar = &m_pTransformer1ModulConfigData->secClampSec;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    dPar = &m_pTransformer1ModulConfigData->dutPrim;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    dPar = &m_pTransformer1ModulConfigData->dutSec;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cTransformer1ModuleConfigData *cTransformer1ModuleConfiguration::getConfigurationData()
{
    return m_pTransformer1ModulConfigData;
}


void cTransformer1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pTransformer1ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setInputModule:
            m_pTransformer1ModulConfigData->m_nModuleId = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSystemCount:
        {
            transformersystemconfiguration tsc;
            tsc.m_sInputPrimaryVector = "ACT_DFTPN1"; // some default
            tsc.m_sInputSecondaryVector = "ACT_DFTPN2";

            m_pTransformer1ModulConfigData->m_nTransformerSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pTransformer1ModulConfigData->m_nTransformerSystemCount; i++)
            {
                m_ConfigXMLMap[QString("trf1modconfpar:configuration:measure:system:trs%1").arg(i+1)] = setMeasSystem1+i;
                m_pTransformer1ModulConfigData->m_transformerSystemConfigList.append(tsc);
                m_pTransformer1ModulConfigData->m_TransformerChannelList.append("");
            }
            break;
        }
        case setSystemUnit:
            m_pTransformer1ModulConfigData->m_clampUnit = m_pXMLReader->getValue(key);
            break;
        case setPrimClampPrim:
            m_pTransformer1ModulConfigData->primClampPrim.m_sKey = key;
            m_pTransformer1ModulConfigData->primClampPrim.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setPrimClampSec:
            m_pTransformer1ModulConfigData->primClampSec.m_sKey = key;
            m_pTransformer1ModulConfigData->primClampSec.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setSecClampPrim:
            m_pTransformer1ModulConfigData->secClampPrim.m_sKey = key;
            m_pTransformer1ModulConfigData->secClampPrim.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setSecClampSec:
            m_pTransformer1ModulConfigData->secClampSec.m_sKey = key;
            m_pTransformer1ModulConfigData->secClampSec.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setPrimDut:
            m_pTransformer1ModulConfigData->dutPrim.m_sKey = key;
            m_pTransformer1ModulConfigData->dutPrim.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        case setSecDut:
            m_pTransformer1ModulConfigData->dutSec.m_sKey = key;
            m_pTransformer1ModulConfigData->dutSec.m_fValue = m_pXMLReader->getValue(key).toDouble();
            break;
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setMeasSystem1) && (cmd < setMeasSystem1 + 12))
            {
                cmd -= setMeasSystem1;
                // it is command for setting measuring mode
                QString trfSystem = m_pXMLReader->getValue(key);
                QStringList trfChannels = trfSystem.split(",");
                transformersystemconfiguration tsc = m_pTransformer1ModulConfigData->m_transformerSystemConfigList.at(cmd);
                tsc.m_sInputPrimaryVector = trfChannels.at(0);
                tsc.m_sInputSecondaryVector = trfChannels.at(1);

                m_pTransformer1ModulConfigData->m_transformerSystemConfigList.replace(cmd, tsc);
                m_pTransformer1ModulConfigData->m_TransformerChannelList.replace(cmd, trfChannels.at(2));
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
    emit configXMLDone();
}

}

