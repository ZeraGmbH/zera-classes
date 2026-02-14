#include "sec1moduleconfiguration.h"
#include "sec1moduleconfigdata.h"
#include <xmlconfigreader.h>

namespace SEC1MODULE
{

cSec1ModuleConfiguration::cSec1ModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSec1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSec1ModuleConfiguration::completeConfiguration);
}


cSec1ModuleConfiguration::~cSec1ModuleConfiguration()
{
    if (m_pSec1ModulConfigData) delete m_pSec1ModulConfigData;
}


void cSec1ModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSec1ModulConfigData) delete m_pSec1ModulConfigData;
    m_pSec1ModulConfigData = new cSec1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["sec1modconfpar:configuration:measure:dutinput:n"] = setDutInputCount;
    m_ConfigXMLMap["sec1modconfpar:configuration:measure:refinput:n"] = setRefInputCount;

    m_ConfigXMLMap["sec1modconfpar:parameter:measure:dutinput"] = setDutInputPar;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:refinput"] = setRefInputPar;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:mode"] = setMeasMode;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:dutconstant"] = setDutConstant;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:dutconstantunit"] = setDutConstantUnit;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:refconstant"] = setRefConstant;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:target"] = setTarget;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:energy"] = setEnergy;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:mrate"] = setMRate;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:continous"] = setContinousMode;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:upperlimit"] = setUpperLimit;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:lowerlimit"] = setLowerLimit;
    m_ConfigXMLMap["sec1modconfpar:parameter:measure:resultunit"] = setResultUnit;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cSec1ModuleConfiguration::exportConfiguration()
{
//    QList<QString> keyList = m_exportEntityList.keys();
//    for (int i = 0; i << keyList.count(); i++)
//        m_pXMLReader->setValue(keyList.at(i), m_exportEntityList[keyList.at(i)]);

    doubleParameter* dPar;
    dPar = &m_pSec1ModulConfigData->m_fDutConstant;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    stringParameter* sPar;
    sPar = &m_pSec1ModulConfigData->m_sDutConstantUnit;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    dPar = &m_pSec1ModulConfigData->m_fRefConstant;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    dPar = &m_pSec1ModulConfigData->m_fEnergy;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    intParameter* iPar;
    iPar = &m_pSec1ModulConfigData->m_nTarget;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nPar));

    iPar = &m_pSec1ModulConfigData->m_nMRate;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nPar));

    sPar = &m_pSec1ModulConfigData->m_sDutInput;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    sPar = &m_pSec1ModulConfigData->m_sRefInput;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    sPar = &m_pSec1ModulConfigData->m_sMode;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    boolParameter* bPar;

    bPar = &m_pSec1ModulConfigData->m_bContinous;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    dPar = &m_pSec1ModulConfigData->m_fUpperLimit;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    dPar = &m_pSec1ModulConfigData->m_fLowerLimit;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    sPar = &m_pSec1ModulConfigData->m_sResultUnit;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSec1ModuleConfigData *cSec1ModuleConfiguration::getConfigurationData()
{
    return m_pSec1ModulConfigData;
}


void cSec1ModuleConfiguration::configXMLInfo(const QString &key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDutInputCount:
            m_pSec1ModulConfigData->m_nDutInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSec1ModulConfigData->m_nDutInpCount; i++) {
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:dutinput:inp%1").arg(i+1)] = setDutInput1Name+i;
                m_pSec1ModulConfigData->m_dutInpList.append(QString());
            }
            break;
        case setRefInputCount:
            m_pSec1ModulConfigData->m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSec1ModulConfigData->m_nRefInpCount; i++) {
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1Name+i;
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:refinput_appends:append%1").arg(i+1)] = setRefInput1Append+i;
                m_pSec1ModulConfigData->m_refInpList.append(TRefInput());
            }
            break;
        case setDutInputPar:
            m_pSec1ModulConfigData->m_sDutInput.m_sKey = key;
            m_pSec1ModulConfigData->m_sDutInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefInputPar:
            m_pSec1ModulConfigData->m_sRefInput.m_sKey = key;
            m_pSec1ModulConfigData->m_sRefInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setMeasMode:
            m_pSec1ModulConfigData->m_sMode.m_sKey = key;
            m_pSec1ModulConfigData->m_sMode.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setDutConstant:
            m_pSec1ModulConfigData->m_fDutConstant.m_sKey = key;
            m_pSec1ModulConfigData->m_fDutConstant.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setDutConstantUnit:
            m_pSec1ModulConfigData->m_sDutConstantUnit.m_sKey = key;
            m_pSec1ModulConfigData->m_sDutConstantUnit.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefConstant:
            m_pSec1ModulConfigData->m_fRefConstant.m_sKey = key;
            m_pSec1ModulConfigData->m_fRefConstant.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setTarget:
            m_pSec1ModulConfigData->m_nTarget.m_sKey = key;
            m_pSec1ModulConfigData->m_nTarget.m_nPar =m_pXMLReader->getValue(key).toUInt(&ok);
            break;
        case setEnergy:
            m_pSec1ModulConfigData->m_fEnergy.m_sKey = key;
            m_pSec1ModulConfigData->m_fEnergy.m_fPar =m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMRate:
            m_pSec1ModulConfigData->m_nMRate.m_sKey = key;
            m_pSec1ModulConfigData->m_nMRate.m_nPar = m_pXMLReader->getValue(key).toUInt(&ok);
            break;
        case setContinousMode:
            m_pSec1ModulConfigData->m_bContinous.m_sKey = key;
            m_pSec1ModulConfigData->m_bContinous.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setUpperLimit:
            m_pSec1ModulConfigData->m_fUpperLimit.m_sKey = key;
            m_pSec1ModulConfigData->m_fUpperLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setLowerLimit:
            m_pSec1ModulConfigData->m_fLowerLimit.m_sKey = key;
            m_pSec1ModulConfigData->m_fLowerLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setResultUnit:
            m_pSec1ModulConfigData->m_sResultUnit.m_sKey = key;
            m_pSec1ModulConfigData->m_sResultUnit.m_sPar = m_pXMLReader->getValue(key);
            break;

        default:
            if ((cmd >= setDutInput1Name) && (cmd < setDutInput1Name + 32)) {
                cmd -= setDutInput1Name;
                QString name = m_pXMLReader->getValue(key);
                m_pSec1ModulConfigData->m_dutInpList.replace(cmd, name);
            }
            else if ((cmd >= setRefInput1Name) && (cmd < setRefInput1Name + 32)) {
                cmd -= setRefInput1Name;
                TRefInput refInput;
                refInput.inputName = m_pXMLReader->getValue(key);
                m_pSec1ModulConfigData->m_refInpList.replace(cmd, refInput);
            }
            else if ((cmd >= setRefInput1Append) && (cmd < setRefInput1Append + 32)) {
                cmd -= setRefInput1Append;
                TRefInput refInput = m_pSec1ModulConfigData->m_refInpList[cmd];
                refInput.nameAppend = m_pXMLReader->getValue(key);
                m_pSec1ModulConfigData->m_refInpList.replace(cmd, refInput);
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
}

}

