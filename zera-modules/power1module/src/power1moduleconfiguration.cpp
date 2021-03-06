#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "power1moduleconfiguration.h"
#include "power1moduleconfigdata.h"
#include "socket.h"

namespace POWER1MODULE
{

cPower1ModuleConfiguration::cPower1ModuleConfiguration()
{
    m_pPower1ModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cPower1ModuleConfiguration::~cPower1ModuleConfiguration()
{
    if (m_pPower1ModulConfigData) delete m_pPower1ModulConfigData;
}


void cPower1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pPower1ModulConfigData) delete m_pPower1ModulConfigData;
    m_pPower1ModulConfigData = new cPower1ModuleConfigData();

    QString s = "m0,m1";
    for (int i = 0; i < 3; i++) // we prepare a list so later we are independent of order of incoming config
        m_pPower1ModulConfigData->m_sMeasSystemList.append(s);

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["pow1modconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;

    m_ConfigXMLMap["pow1modconfpar:configuration:measure:modes:n"] = setMeasModeCount;

    m_ConfigXMLMap["pow1modconfpar:configuration:measure:system:pms1"] = setMeasSystem1;
    m_ConfigXMLMap["pow1modconfpar:configuration:measure:system:pms2"] = setMeasSystem2;
    m_ConfigXMLMap["pow1modconfpar:configuration:measure:system:pms3"] = setMeasSystem3;
    m_ConfigXMLMap["pow1modconfpar:configuration:measure:system:pms2w"] = set2WMeasSystem;

    m_ConfigXMLMap["pow1modconfpar:configuration:measure:integrationmode"] = setIntegrationMode;
    m_ConfigXMLMap["pow1modconfpar:configuration:measure:movingwindow:on"] = setMovingwindowBool;
    m_ConfigXMLMap["pow1modconfpar:configuration:measure:movingwindow:time"] = setMovingwindowTime;

    m_ConfigXMLMap["pow1modconfpar:configuration:frequencyoutput:frequency"] = setNominalFrequency;
    m_ConfigXMLMap["pow1modconfpar:configuration:frequencyoutput:frequencyact"] = setFrequencyActualizationMode;
    m_ConfigXMLMap["pow1modconfpar:configuration:frequencyoutput:output:n"] = setFrequencyOutputCount;

    m_ConfigXMLMap["pow1modconfpar:parameter:measuringmode"] = setMeasuringMode;
    m_ConfigXMLMap["pow1modconfpar:parameter:interval:time"] = setMeasureIntervalTime;
    m_ConfigXMLMap["pow1modconfpar:parameter:interval:period"] = setMeasureIntervalPeriod;


    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cPower1ModuleConfiguration::exportConfiguration()
{
    doubleParameter* dPar;
    dPar = &m_pPower1ModulConfigData->m_fMeasIntervalTime;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));
    intParameter* iPar;
    iPar = &m_pPower1ModulConfigData->m_nMeasIntervalPeriod;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nValue));
    stringParameter* sPar;
    sPar = &m_pPower1ModulConfigData->m_sMeasuringMode;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sValue);
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cPower1ModuleConfigData *cPower1ModuleConfiguration::getConfigurationData()
{
    return m_pPower1ModulConfigData;
}


void cPower1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pPower1ModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMIp:
            m_pPower1ModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pPower1ModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pPower1ModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pPower1ModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pPower1ModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pPower1ModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setMeasModeCount:
            m_pPower1ModulConfigData->m_nMeasModeCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pPower1ModulConfigData->m_nMeasModeCount; i++)
                m_ConfigXMLMap[QString("pow1modconfpar:configuration:measure:modes:m%1").arg(i+1)] = setMeasMode1+i;
            break;
        case setMeasSystem1:
            m_pPower1ModulConfigData->m_sMeasSystemList.replace(0, m_pXMLReader->getValue(key));
            break;
        case setMeasSystem2:
            m_pPower1ModulConfigData->m_sMeasSystemList.replace(1, m_pXMLReader->getValue(key));
            break;
        case setMeasSystem3:
            m_pPower1ModulConfigData->m_sMeasSystemList.replace(2, m_pXMLReader->getValue(key));
            break;
        case set2WMeasSystem:
            m_pPower1ModulConfigData->m_sM2WSystem = m_pXMLReader->getValue(key);
            break;
        case setIntegrationMode:
            m_pPower1ModulConfigData->m_sIntegrationMode = m_pXMLReader->getValue(key);
            break;
        case setMovingwindowBool:
            m_pPower1ModulConfigData->m_bmovingWindow = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMovingwindowTime:
            m_pPower1ModulConfigData->m_fmovingwindowInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setNominalFrequency:
            m_pPower1ModulConfigData->m_nNominalFrequency = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setFrequencyActualizationMode:
            m_pPower1ModulConfigData->m_sFreqActualizationMode = m_pXMLReader->getValue(key);
            break;
        case setFrequencyOutputCount:
            m_pPower1ModulConfigData->m_nFreqOutputCount = m_pXMLReader->getValue(key).toInt(&ok);
            if (m_pPower1ModulConfigData->m_nFreqOutputCount > 0)
                for (int i = 0; i < m_pPower1ModulConfigData->m_nFreqOutputCount; i++)
                {
                    m_ConfigXMLMap[QString("pow1modconfpar:configuration:frequencyoutput:output:fout%1:name").arg(i+1)] = setfreqout1Name+i;
                    m_ConfigXMLMap[QString("pow1modconfpar:configuration:frequencyoutput:output:fout%1:source").arg(i+1)] = setfreqout1Source+i;
                    m_ConfigXMLMap[QString("pow1modconfpar:configuration:frequencyoutput:output:fout%1:type").arg(i+1)] = setfreqout1Type+i;
                    m_ConfigXMLMap[QString("pow1modconfpar:configuration:frequencyoutput:output:fout%1:plug").arg(i+1)] = setfreqout1Plug+i;
                    freqoutconfiguration fconf;
                    m_pPower1ModulConfigData->m_FreqOutputConfList.append(fconf);
                }
            break;

        case setMeasuringMode:
            m_pPower1ModulConfigData->m_sMeasuringMode.m_sKey = key;
            m_pPower1ModulConfigData->m_sMeasuringMode.m_sValue = m_pXMLReader->getValue(key);
            break;
        case setMeasureIntervalTime:
            m_pPower1ModulConfigData->m_fMeasIntervalTime.m_sKey = key;
            m_pPower1ModulConfigData->m_fMeasIntervalTime.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMeasureIntervalPeriod:
            m_pPower1ModulConfigData->m_nMeasIntervalPeriod.m_sKey = key;
            m_pPower1ModulConfigData->m_nMeasIntervalPeriod.m_nValue = m_pXMLReader->getValue(key).toInt(&ok);
            break;

        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setMeasMode1) && (cmd < setMeasMode1 + 32))
            {
                cmd -= setMeasMode1;
                // it is command for setting measuring mode
                QString measMode = m_pXMLReader->getValue(key);
                m_pPower1ModulConfigData->m_sMeasmodeList.append(measMode); // for configuration of our engine
            }

            else

            if ((cmd >= setfreqout1Name) && (cmd < setfreqout1Name + 8))
            {
                cmd -= setfreqout1Name;
                freqoutconfiguration fconf;
                fconf = m_pPower1ModulConfigData->m_FreqOutputConfList.at(cmd);
                fconf.m_sName = m_pXMLReader->getValue(key);
                m_pPower1ModulConfigData->m_FreqOutputConfList.replace(cmd, fconf);
            }

            else

            if ((cmd >= setfreqout1Source) && (cmd < setfreqout1Source + 8))
            {
                QString s;
                s = m_pXMLReader->getValue(key);
                cmd -= setfreqout1Source;
                freqoutconfiguration fconf;
                fconf = m_pPower1ModulConfigData->m_FreqOutputConfList.at(cmd);
                if (s == "pms1")
                    fconf.m_nSource = 0;
                if (s == "pms2")
                    fconf.m_nSource = 1;
                if (s == "pms3")
                    fconf.m_nSource = 2;
                if (s == "pmss")
                    fconf.m_nSource = 3;

                m_pPower1ModulConfigData->m_FreqOutputConfList.replace(cmd, fconf);
            }

            else

            if ((cmd >= setfreqout1Type) && (cmd < setfreqout1Type+ 8))
            {
                QString s;
                cmd -= setfreqout1Type;
                s = m_pXMLReader->getValue(key);
                freqoutconfiguration fconf;
                fconf = m_pPower1ModulConfigData->m_FreqOutputConfList.at(cmd);
                if (s == "+-")
                    fconf.m_nFoutMode = absPower;
                if (s == "+")
                    fconf.m_nFoutMode = posPower;
                if (s == "-")
                    fconf.m_nFoutMode = negPower;

                m_pPower1ModulConfigData->m_FreqOutputConfList.replace(cmd, fconf);
            }

            else

            if ((cmd >= setfreqout1Plug) && (cmd < setfreqout1Plug + 8))
            {
                cmd -= setfreqout1Plug;
                freqoutconfiguration fconf;
                fconf = m_pPower1ModulConfigData->m_FreqOutputConfList.at(cmd);
                fconf.m_sPlug = m_pXMLReader->getValue(key);
                m_pPower1ModulConfigData->m_FreqOutputConfList.replace(cmd, fconf);
            }

        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cPower1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

