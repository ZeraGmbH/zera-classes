#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "rangemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"
#include "socket.h"

namespace RANGEMODULE
{

cRangeModuleConfiguration::cRangeModuleConfiguration()
{
    m_pRangeModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cRangeModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cRangeModuleConfiguration::completeConfiguration);
}


cRangeModuleConfiguration::~cRangeModuleConfiguration()
{
    if (m_pRangeModulConfigData) delete m_pRangeModulConfigData;
}


void cRangeModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pRangeModulConfigData) delete m_pRangeModulConfigData;
    m_pRangeModulConfigData = new cRangeModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:session"] = setSession;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:resourcemanager:ip"] = setRMIp;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:resourcemanager:port"] = setRMPort;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:pcbserver:ip"] = setPCBServerIp;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:pcbserver:port"] = setPCBServerPort;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:dspserver:ip"] = setDSPServerIp;
    m_ConfigXMLMap["rangemodconfpar:configuration:connectivity:ethernet:dspserver:port"] = setDSPServerPort;

    m_ConfigXMLMap["rangemodconfpar:configuration:sense:channel:n"] = setChannelCount;
    m_ConfigXMLMap["rangemodconfpar:configuration:sense:subdc:n"] = setSubdcCount;
    m_ConfigXMLMap["rangemodconfpar:configuration:sense:group:n"] = setGroupCount;

    m_ConfigXMLMap["rangemodconfpar:configuration:measure:interval"] = setMeasureInterval;
    m_ConfigXMLMap["rangemodconfpar:configuration:adjustment:interval"] = setAdjustInterval;
    m_ConfigXMLMap["rangemodconfpar:configuration:adjustment:ignoreRmsValues:enable"] = setAdjustIgnoreRmsValuesEnable;
    m_ConfigXMLMap["rangemodconfpar:configuration:adjustment:ignoreRmsValues:threshold"] = setAdjustIgnoreRmsValuesThreshold;

    m_ConfigXMLMap["rangemodconfpar:parameter:sense:grouping"] = setGrouping;
    m_ConfigXMLMap["rangemodconfpar:parameter:sense:rangeauto"] = setRangeAutomatic;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cRangeModuleConfiguration::exportConfiguration()
{
//    QList<QString> keyList = m_exportEntityList.keys();
//    for (int i = 0; i << keyList.count(); i++)
//        m_pXMLReader->setValue(keyList.at(i), m_exportEntityList[keyList.at(i)]);

    boolParameter* bPar;
    bPar = &m_pRangeModulConfigData->m_ObsermaticConfPar.m_nGroupAct;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));
    bPar = &m_pRangeModulConfigData->m_ObsermaticConfPar.m_nRangeAutoAct;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    stringParameter sPar;
    QList<stringParameter> sParList = m_pRangeModulConfigData->m_ObsermaticConfPar.m_senseChannelRangeParameter;

    boolParameter invertPhaseState;
    QList<boolParameter> invertPhaseStateList = m_pRangeModulConfigData->m_adjustConfPar.m_senseChannelInvertParameter;
    for (int i = 0; i < sParList.count(); i++)
    {
        sPar = sParList.at(i);
        m_pXMLReader->setValue(sPar.m_sKey, sPar.m_sPar);

        invertPhaseState = invertPhaseStateList.at(i);
        m_pXMLReader->setValue(invertPhaseState.m_sKey, QString("%1").arg(invertPhaseState.m_nActive));
    }

    bPar = &m_pRangeModulConfigData->m_adjustConfPar.m_ignoreRmsValuesEnable;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));
    doubleParameter* dPar = &m_pRangeModulConfigData->m_adjustConfPar.m_ignoreRmsValuesThreshold;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fValue));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cRangeModuleConfigData *cRangeModuleConfiguration::getConfigurationData()
{
    return m_pRangeModulConfigData;
}


void cRangeModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;
    int n;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setSession:
            m_pRangeModulConfigData->m_session.m_sKey = key;
            m_pRangeModulConfigData->m_session.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRMIp:
            m_pRangeModulConfigData->m_RMSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setRMPort:
            m_pRangeModulConfigData->m_RMSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setPCBServerIp:
            m_pRangeModulConfigData->m_PCBServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setPCBServerPort:
            m_pRangeModulConfigData->m_PCBServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setDSPServerIp:
            m_pRangeModulConfigData->m_DSPServerSocket.m_sIP = m_pXMLReader->getValue(key);
            break;
        case setDSPServerPort:
            m_pRangeModulConfigData->m_DSPServerSocket.m_nPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setChannelCount:
        {
            m_pRangeModulConfigData->m_nChannelCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for channel configuration
            stringParameter sParam;
            boolParameter bParam;
            for (int i = 0; i < m_pRangeModulConfigData->m_nChannelCount; i++)
            {
                m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:channel:ch%1").arg(i+1)] = setSenseChannel1+i;
                // m_senseChannelRangeList.append("");
                m_pRangeModulConfigData->m_ObsermaticConfPar.m_senseChannelRangeParameter.append(sParam);
                m_pRangeModulConfigData->m_adjustConfPar.m_senseChannelInvertParameter.append(bParam);
            }
            break;
        }
        case setSubdcCount:
        {
            m_pRangeModulConfigData->m_nSubDCCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for channel configuration
            if (m_pRangeModulConfigData->m_nSubDCCount > 0)
            for (int i = 0; i < m_pRangeModulConfigData->m_nSubDCCount; i++)
            {
                m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:subdc:ch%1").arg(i+1)] = setSubdcChannel1+i;
            }
            break;
        }
        case setGroupCount:
            m_pRangeModulConfigData->m_nGroupCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for group configuration
            for (int i = 0; i < m_pRangeModulConfigData->m_nGroupCount; i++)
            {
                 m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:group:gr%1:n").arg(i+1)] = setGroup1ChannelCount+i;
                 m_pRangeModulConfigData->m_GroupCountList.append(n=0); // and the needed lists
                 m_pRangeModulConfigData->m_GroupList.append(QStringList());
            }
            break;
        case setGrouping:
            m_pRangeModulConfigData->m_ObsermaticConfPar.m_nGroupAct.m_sKey = key;
            m_pRangeModulConfigData->m_ObsermaticConfPar.m_nGroupAct.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRangeAutomatic:
            m_pRangeModulConfigData->m_ObsermaticConfPar.m_nRangeAutoAct.m_sKey = key;
            m_pRangeModulConfigData->m_ObsermaticConfPar.m_nRangeAutoAct.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setMeasureInterval:
            m_pRangeModulConfigData->m_fMeasInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setAdjustInterval:
            m_pRangeModulConfigData->m_adjustConfPar.m_fAdjInterval = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setAdjustIgnoreRmsValuesEnable:
            m_pRangeModulConfigData->m_adjustConfPar.m_ignoreRmsValuesEnable.m_sKey = key;
            m_pRangeModulConfigData->m_adjustConfPar.m_ignoreRmsValuesEnable.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setAdjustIgnoreRmsValuesThreshold:
            m_pRangeModulConfigData->m_adjustConfPar.m_ignoreRmsValuesThreshold.m_sKey = key;
            m_pRangeModulConfigData->m_adjustConfPar.m_ignoreRmsValuesThreshold.m_fValue = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        default:
            if ((cmd >= setDefaultRange1) && (cmd < setDefaultRange1 + 32)) {
                cmd -= setDefaultRange1;
                //m_pRangeModulConfigData->m_senseChannelRangeList.replace(cmd, m_pXMLReader->getValue(key));
                stringParameter sParam;
                sParam.m_sKey = key;
                sParam.m_sPar = m_pXMLReader->getValue(key);
                m_pRangeModulConfigData->m_ObsermaticConfPar.m_senseChannelRangeParameter.replace(cmd, sParam);
            }
            else if ((cmd >= setInvertChannel1) && (cmd < setInvertChannel1 + 32)) {
                cmd -= setInvertChannel1;
                //m_pRangeModulConfigData->m_senseChannelRangeList.replace(cmd, m_pXMLReader->getValue(key));
                boolParameter bParam;
                bParam.m_sKey = key;
                bParam.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
                m_pRangeModulConfigData->m_adjustConfPar.m_senseChannelInvertParameter.replace(cmd, bParam);
            }
            else if ((cmd >= setSenseChannel1) && (cmd < setSenseChannel1 + m_pRangeModulConfigData->m_nChannelCount)) {
                cmd -= setSenseChannel1;
                // it is command for setting channel name
                QString senseChannel = m_pXMLReader->getValue(key);
                m_pRangeModulConfigData->m_senseChannelList.append(senseChannel); // for configuration of our engine
                m_ConfigXMLMap[QString("rangemodconfpar:parameter:sense:%1:range").arg(senseChannel)] = setDefaultRange1+cmd;
                m_ConfigXMLMap[QString("rangemodconfpar:parameter:sense:%1:invert").arg(senseChannel)] = setInvertChannel1+cmd;
            }
            else if ((cmd >= setSubdcChannel1) && (cmd < setSubdcChannel1 + m_pRangeModulConfigData->m_nSubDCCount)) {
                cmd -= setSubdcChannel1;
                // it is command for setting subdc channel name
                QString subdcChannel = m_pXMLReader->getValue(key);
                m_pRangeModulConfigData->m_subdcChannelList.append(subdcChannel); // for configuration of our engine
            }
            else if ((cmd >= setGroup1ChannelCount) && (cmd < (setGroup1ChannelCount+ m_pRangeModulConfigData->m_nGroupCount))) {
                // it is cmd for setting groupx channel count
                cmd -= setGroup1ChannelCount;
                m_pRangeModulConfigData->m_GroupCountList.replace(cmd , m_pXMLReader->getValue(key).toInt(&ok));
                for (int i = 0; i < m_pRangeModulConfigData->m_GroupCountList.at(cmd); i++) // create hash entries for setting channels in this group
                    m_ConfigXMLMap[QString("rangemodconfpar:configuration:sense:group:gr%1:ch%2").arg(cmd+1).arg(i+1)] = setGroup1Channel1 +(cmd << 5) +i;
            }
            else {
                for (int i = 0; i < m_pRangeModulConfigData->m_nGroupCount; i++) {
                    if ((cmd >= setGroup1Channel1 + (i << 5)) && (cmd < (setGroup1Channel1+ (i << 5) + 32))) {
                        QStringList sl = m_pRangeModulConfigData->m_GroupList.at(i);
                        sl.append(m_pXMLReader->getValue(key));
                        m_pRangeModulConfigData->m_GroupList.replace(i, sl);
                    }
                }
            }
        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cRangeModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

