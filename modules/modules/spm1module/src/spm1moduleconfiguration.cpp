#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "spm1moduleconfiguration.h"
#include "spm1moduleconfigdata.h"
#include "socket.h"

namespace SPM1MODULE
{

cSpm1ModuleConfiguration::cSpm1ModuleConfiguration()
{
    m_pSpm1ModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSpm1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSpm1ModuleConfiguration::completeConfiguration);
}


cSpm1ModuleConfiguration::~cSpm1ModuleConfiguration()
{
    if (m_pSpm1ModulConfigData) delete m_pSpm1ModulConfigData;
}


void cSpm1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSpm1ModulConfigData) delete m_pSpm1ModulConfigData;
    m_pSpm1ModulConfigData = new cSpm1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:ethernet:secserver:ip"] = setSPMServerIp;
    m_ConfigXMLMap["spm1modconfpar:configuration:connectivity:ethernet:secserver:port"] = setSPMServerPort;

    m_ConfigXMLMap["spm1modconfpar:configuration:measure:refinput:n"] = setRefInputCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:activeunits:n"] = setActiveUnitCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:reactiveunits:n"] = setReactiveUnitCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:apparentunits:n"] = setApparentUnitCount;
    m_ConfigXMLMap["spm1modconfpar:configuration:measure:embedded"] = setEmbedded;

    m_ConfigXMLMap["spm1modconfpar:parameter:measure:refinput"] = setRefInput;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:targeted"] = setTargeted;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:meastime"] = setMeasTime;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:upperlimit"] = setUpperLimit;
    m_ConfigXMLMap["spm1modconfpar:parameter:measure:lowerlimit"] = setLowerLimit;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSpm1ModuleConfiguration::exportConfiguration()
{
    stringParameter* sPar;
    sPar = &m_pSpm1ModulConfigData->m_sRefInput;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    boolParameter* bPar;
    bPar = &m_pSpm1ModulConfigData->m_bTargeted;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    intParameter* iPar;
    iPar = &m_pSpm1ModulConfigData->m_nMeasTime;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nPar));

    doubleParameter* dPar;
    dPar = &m_pSpm1ModulConfigData->m_fUpperLimit;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    dPar = &m_pSpm1ModulConfigData->m_fLowerLimit;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSpm1ModuleConfigData *cSpm1ModuleConfiguration::getConfigurationData()
{
    return m_pSpm1ModulConfigData;
}


void cSpm1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pSpm1ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pSpm1ModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pSpm1ModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pSpm1ModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pSpm1ModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSPMServerIp:
            m_pSpm1ModulConfigData->m_SECServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setSPMServerPort:
            m_pSpm1ModulConfigData->m_SECServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRefInputCount:
        {
            QString name;
            m_pSpm1ModulConfigData->m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            if (m_pSpm1ModulConfigData->m_nRefInpCount > 0)
            for (int i = 0; i < m_pSpm1ModulConfigData->m_nRefInpCount; i++)
                {
                    m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1Name+i;
                    m_pSpm1ModulConfigData->m_refInpList.append(name);
                }
            break;
        }
        case setActiveUnitCount:
        {
            QString name;
            m_pSpm1ModulConfigData->m_nActiveUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            if (m_pSpm1ModulConfigData->m_nActiveUnitCount > 0)
            for (int i = 0; i < m_pSpm1ModulConfigData->m_nActiveUnitCount; i++)
                {
                    m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:activeunits:unit%1").arg(i+1)] = setActiveUnit1Name+i;
                    m_pSpm1ModulConfigData->m_ActiveUnitList.append(name);
                }
            break;
        }
        case setReactiveUnitCount:
        {
            QString name;
            m_pSpm1ModulConfigData->m_nReactiveUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            if (m_pSpm1ModulConfigData->m_nReactiveUnitCount > 0)
            for (int i = 0; i < m_pSpm1ModulConfigData->m_nReactiveUnitCount; i++)
                {
                    m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:reactiveunits:unit%1").arg(i+1)] = setReactiveUnit1Name+i;
                    m_pSpm1ModulConfigData->m_ReactiveUnitList.append(name);
                }
            break;
        }
        case setApparentUnitCount:
        {
            QString name;
            m_pSpm1ModulConfigData->m_nApparentUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            if (m_pSpm1ModulConfigData->m_nApparentUnitCount > 0)
            for (int i = 0; i < m_pSpm1ModulConfigData->m_nApparentUnitCount; i++)
                {
                    m_ConfigXMLMap[QString("spm1modconfpar:configuration:measure:apparentunits:unit%1").arg(i+1)] = setApparentUnit1Name+i;
                    m_pSpm1ModulConfigData->m_ApparentUnitList.append(name);
                }
            break;
        }
        case setEmbedded:
            m_pSpm1ModulConfigData->m_bEmbedded = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setRefInput:
            m_pSpm1ModulConfigData->m_sRefInput.m_sKey = key;
            m_pSpm1ModulConfigData->m_sRefInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setTargeted:
            m_pSpm1ModulConfigData->m_bTargeted.m_sKey = key;
            m_pSpm1ModulConfigData->m_bTargeted.m_nActive = m_pXMLReader->getValue(key).toInt();
            break;
        case setMeasTime:
            m_pSpm1ModulConfigData->m_nMeasTime.m_sKey = key;
            m_pSpm1ModulConfigData->m_nMeasTime.m_nPar = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setUpperLimit:
            m_pSpm1ModulConfigData->m_fUpperLimit.m_sKey = key;
            m_pSpm1ModulConfigData->m_fUpperLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setLowerLimit:
            m_pSpm1ModulConfigData->m_fLowerLimit.m_sKey = key;
            m_pSpm1ModulConfigData->m_fLowerLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;



        default:
            QString name;
            if ((cmd >= setRefInput1Name) && (cmd < setRefInput1Name + 32))
            {
                cmd -= setRefInput1Name;
                name = m_pXMLReader->getValue(key);
                m_pSpm1ModulConfigData->m_refInpList.replace(cmd, name);
            }
            else
                if ((cmd >= setActiveUnit1Name) && (cmd < setActiveUnit1Name + 16))
                {
                    cmd -= setActiveUnit1Name;
                    name = m_pXMLReader->getValue(key);
                    m_pSpm1ModulConfigData->m_ActiveUnitList.replace(cmd, name);
                }
                else
                    if ((cmd >= setReactiveUnit1Name) && (cmd < setReactiveUnit1Name + 16))
                    {
                        cmd -= setReactiveUnit1Name;
                        name = m_pXMLReader->getValue(key);
                        m_pSpm1ModulConfigData->m_ReactiveUnitList.replace(cmd, name);
                    }
                    else
                        if ((cmd >= setApparentUnit1Name) && (cmd < setApparentUnit1Name + 16))
                        {
                            cmd -= setApparentUnit1Name;
                            name = m_pXMLReader->getValue(key);
                            m_pSpm1ModulConfigData->m_ApparentUnitList.replace(cmd, name);
                        }
            break;
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cSpm1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

