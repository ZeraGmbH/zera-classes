#include "sec1moduleconfiguration.h"

namespace SEC1MODULE
{

cSec1ModuleConfiguration::cSec1ModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setDutInputCount,
    setRefInputCount,

    setDutInputPar,
    setRefInputPar,
    setMeasMode,
    setDutConstant,
    setDutConstantUnit,
    setRefConstant,
    setTarget,
    setEnergy,
    setMRate,
    setContinousMode,
    setUpperLimit,
    setLowerLimit,
    setResultUnit,

    setDutInput1Name = 32,
    setRefInput1Name = 64,
    setRefInput1Append = 96
};

void cSec1ModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
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

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSec1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSec1ModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cSec1ModuleConfiguration::exportConfiguration() const
{
    const doubleParameter* paramDutConstant = &m_configData.m_fDutConstant;
    m_pXMLReader->setValue(paramDutConstant->m_sKey, QString("%1").arg(paramDutConstant->m_fPar));

    const stringParameter* paramDutConstantUnit = &m_configData.m_sDutConstantUnit;
    m_pXMLReader->setValue(paramDutConstantUnit->m_sKey, paramDutConstantUnit->m_sPar);

    const doubleParameter* paramRefConstant = &m_configData.m_fRefConstant;
    m_pXMLReader->setValue(paramRefConstant->m_sKey, QString("%1").arg(paramRefConstant->m_fPar));

    const doubleParameter* paramEnergy = &m_configData.m_fEnergy;
    m_pXMLReader->setValue(paramEnergy->m_sKey, QString("%1").arg(paramEnergy->m_fPar));

    const intParameter* paramTarget = &m_configData.m_nTarget;
    m_pXMLReader->setValue(paramTarget->m_sKey, QString("%1").arg(paramTarget->m_nPar));

    const intParameter* paramRate = &m_configData.m_nMRate;
    m_pXMLReader->setValue(paramRate->m_sKey, QString("%1").arg(paramRate->m_nPar));

    const stringParameter* paramDutInput = &m_configData.m_sDutInput;
    m_pXMLReader->setValue(paramDutInput->m_sKey, paramDutInput->m_sPar);

    const stringParameter* paramRefInput = &m_configData.m_sRefInput;
    m_pXMLReader->setValue(paramRefInput->m_sKey, paramRefInput->m_sPar);

    const stringParameter* paramNode = &m_configData.m_sMode;
    m_pXMLReader->setValue(paramNode->m_sKey, paramNode->m_sPar);

    const boolParameter* paramContinous = &m_configData.m_bContinous;
    m_pXMLReader->setValue(paramContinous->m_sKey, QString("%1").arg(paramContinous->m_nActive));

    const doubleParameter* paramUpperLimit = &m_configData.m_fUpperLimit;
    m_pXMLReader->setValue(paramUpperLimit->m_sKey, QString("%1").arg(paramUpperLimit->m_fPar));

    const doubleParameter* paramLowerLimit = &m_configData.m_fLowerLimit;
    m_pXMLReader->setValue(paramLowerLimit->m_sKey, QString("%1").arg(paramLowerLimit->m_fPar));

    const stringParameter* paramResultUnit = &m_configData.m_sResultUnit;
    m_pXMLReader->setValue(paramResultUnit->m_sKey, paramResultUnit->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSec1ModuleConfigData *cSec1ModuleConfiguration::getConfigData()
{
    return &m_configData;
}


void cSec1ModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDutInputCount:
            m_configData.m_nDutInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nDutInpCount; i++) {
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:dutinput:inp%1").arg(i+1)] = setDutInput1Name+i;
                m_configData.m_dutInpList.append(QString());
            }
            break;
        case setRefInputCount:
            m_configData.m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_nRefInpCount; i++) {
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1Name+i;
                m_ConfigXMLMap[QString("sec1modconfpar:configuration:measure:refinput_appends:append%1").arg(i+1)] = setRefInput1Append+i;
                m_configData.m_refInpList.append(TRefInput());
            }
            break;
        case setDutInputPar:
            m_configData.m_sDutInput.m_sKey = key;
            m_configData.m_sDutInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefInputPar:
            m_configData.m_sRefInput.m_sKey = key;
            m_configData.m_sRefInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setMeasMode:
            m_configData.m_sMode.m_sKey = key;
            m_configData.m_sMode.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setDutConstant:
            m_configData.m_fDutConstant.m_sKey = key;
            m_configData.m_fDutConstant.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setDutConstantUnit:
            m_configData.m_sDutConstantUnit.m_sKey = key;
            m_configData.m_sDutConstantUnit.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setRefConstant:
            m_configData.m_fRefConstant.m_sKey = key;
            m_configData.m_fRefConstant.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setTarget:
            m_configData.m_nTarget.m_sKey = key;
            m_configData.m_nTarget.m_nPar =m_pXMLReader->getValue(key).toUInt(&ok);
            break;
        case setEnergy:
            m_configData.m_fEnergy.m_sKey = key;
            m_configData.m_fEnergy.m_fPar =m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setMRate:
            m_configData.m_nMRate.m_sKey = key;
            m_configData.m_nMRate.m_nPar = m_pXMLReader->getValue(key).toUInt(&ok);
            break;
        case setContinousMode:
            m_configData.m_bContinous.m_sKey = key;
            m_configData.m_bContinous.m_nActive = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setUpperLimit:
            m_configData.m_fUpperLimit.m_sKey = key;
            m_configData.m_fUpperLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setLowerLimit:
            m_configData.m_fLowerLimit.m_sKey = key;
            m_configData.m_fLowerLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setResultUnit:
            m_configData.m_sResultUnit.m_sKey = key;
            m_configData.m_sResultUnit.m_sPar = m_pXMLReader->getValue(key);
            break;

        default:
            if ((cmd >= setDutInput1Name) && (cmd < setDutInput1Name + 32)) {
                cmd -= setDutInput1Name;
                QString name = m_pXMLReader->getValue(key);
                m_configData.m_dutInpList.replace(cmd, name);
            }
            else if ((cmd >= setRefInput1Name) && (cmd < setRefInput1Name + 32)) {
                cmd -= setRefInput1Name;
                TRefInput refInput;
                refInput.inputName = m_pXMLReader->getValue(key);
                m_configData.m_refInpList.replace(cmd, refInput);
            }
            else if ((cmd >= setRefInput1Append) && (cmd < setRefInput1Append + 32)) {
                cmd -= setRefInput1Append;
                TRefInput refInput = m_configData.m_refInpList[cmd];
                refInput.nameAppend = m_pXMLReader->getValue(key);
                m_configData.m_refInpList.replace(cmd, refInput);
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

