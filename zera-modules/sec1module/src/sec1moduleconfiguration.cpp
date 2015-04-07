#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "sec1moduleconfiguration.h"
#include "sec1moduleconfigdata.h"
#include "socket.h"

namespace SEC1MODULE
{

cSec1ModuleConfiguration::cSec1ModuleConfiguration()
{
    m_pSec1ModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cSec1ModuleConfiguration::~cSec1ModuleConfiguration()
{
    if (m_pSec1ModulConfigData) delete m_pSec1ModulConfigData;
}


void cSec1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSec1ModulConfigData) delete m_pSec1ModulConfigData;
    m_pSec1ModulConfigData = new cSec1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:ethernet:secserver:ip"] = setSECServerIp;
    m_ConfigXMLMap["sec1modconfpar:configuration:connectivity:ethernet:secserver:port"] = setSECServerPort;

    m_ConfigXMLMap["sec1modconfpar:configuration:measure:refconstantnotifier"] = setwithrefconstantnotifier;
    m_ConfigXMLMap["sec1modconfpar:configuration:measure:dutinput:n"] = setDutInputCount;
    m_ConfigXMLMap["sec1modconfpar:configuration:measure:refinput:n"] = setRefInputCount;
    m_ConfigXMLMap["sec1modconfpar:configuration:measure:interval"] = setMeasureInterval;

    m_ConfigXMLMap["sec1modconfpar:parameter:dutinput"] = setDutInputPar;
    m_ConfigXMLMap["sec1modconfpar:parameter:refinput"] = setRefInputPar;
    m_ConfigXMLMap["sec1modconfpar:parameter:dutconstant"] = setDutConstant;
    m_ConfigXMLMap["sec1modconfpar:parameter:refconstant"] = setRefConstant;
    m_ConfigXMLMap["sec1modconfpar:parameter:targetvalue"] = setTargetValue;
    m_ConfigXMLMap["sec1modconfpar:parameter:measpulses"] = setMeasPulses;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSec1ModuleConfiguration::exportConfiguration()
{
//    QList<QString> keyList = m_exportEntityList.keys();
//    for (int i = 0; i << keyList.count(); i++)
//        m_pXMLReader->setValue(keyList.at(i), m_exportEntityList[keyList.at(i)]);

    doubleParameter* dPar;
    dPar = &m_pSec1ModulConfigData->m_fDutConstant;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    dPar = &m_pSec1ModulConfigData->m_fRefConstant;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    intParameter* iPar;
    iPar = &m_pSec1ModulConfigData->m_nTargetValue;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nPar));

    iPar = &m_pSec1ModulConfigData->m_nMeasPulses;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nPar));

    stringParameter* sPar;
    sPar = &m_pSec1ModulConfigData->m_sDutInput;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    sPar = &m_pSec1ModulConfigData->m_sRefInput;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSec1ModuleConfigData *cSec1ModuleConfiguration::getConfigurationData()
{
    return m_pSec1ModulConfigData;
}


void cSec1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pSec1ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pSec1ModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pSec1ModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pSec1ModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pSec1ModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSECServerIp:
            m_pSec1ModulConfigData->m_SECServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setSECServerPort:
            m_pSec1ModulConfigData->m_SECServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setwithrefconstantnotifier:
            m_pSec1ModulConfigData->m_bwithrefconstantnotifier = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setDutInputCount:
            m_pSec1ModulConfigData->m_nDutInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSec1ModulConfigData->m_nDutInpCount; i++)
            {
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:dutinput:inp%1").arg(i+1)] = setDutInput1+i;
                m_pSec1ModulConfigData->m_dutInpList.append("");
            }
            break;
        case setRefInputCount:
            m_pSec1ModulConfigData->m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSec1ModulConfigData->m_nRefInpCount; i++)
            {
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1+i;
                m_pSec1ModulConfigData->m_refInpList.append("");
            }
            break;
        case setMeasureInterval:
            m_pSec1ModulConfigData->m_fMeasInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setDutInputPar:
            m_pSec1ModulConfigData->m_sDutInput.m_sKey = key;
            m_pSec1ModulConfigData->m_sDutInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefInputPar:
            m_pSec1ModulConfigData->m_sRefInput.m_sKey = key;
            m_pSec1ModulConfigData->m_sRefInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setDutConstant:
            m_pSec1ModulConfigData->m_fDutConstant.m_sKey = key;
            m_pSec1ModulConfigData->m_fDutConstant.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setRefConstant:
            m_pSec1ModulConfigData->m_fRefConstant.m_sKey = key;
            m_pSec1ModulConfigData->m_fRefConstant.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasPulses:
            m_pSec1ModulConfigData->m_nMeasPulses.m_sKey = key;
            m_pSec1ModulConfigData->m_nMeasPulses.m_nPar = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setTargetValue:
            m_pSec1ModulConfigData->m_nTargetValue.m_sKey = key;
            m_pSec1ModulConfigData->m_nTargetValue.m_nPar =m_pXMLReader->getValue(key).toInt(&ok);
            break;
        default:
            if ((cmd >= setDutInput1) && (cmd < setDutInput1 + 32))
            {
                cmd -= setDutInput1;
                m_pSec1ModulConfigData->m_dutInpList.replace(cmd, m_pXMLReader->getValue(key));
            }
            else
                if ((cmd >= setRefInput1) && (cmd < setRefInput1 + 32))
                {
                    cmd -= setRefInput1;
                    m_pSec1ModulConfigData->m_refInpList.replace(cmd, m_pXMLReader->getValue(key));
                }
            break;
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cSec1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

