#include "sem1moduleconfiguration.h"
#include "sem1moduleconfigdata.h"
#include <xmlconfigreader.h>

namespace SEM1MODULE
{

cSem1ModuleConfiguration::cSem1ModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cSem1ModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cSem1ModuleConfiguration::completeConfiguration);
}


cSem1ModuleConfiguration::~cSem1ModuleConfiguration()
{
    if (m_pSem1ModulConfigData) delete m_pSem1ModulConfigData;
}


void cSem1ModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pSem1ModulConfigData) delete m_pSem1ModulConfigData;
    m_pSem1ModulConfigData = new cSem1ModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["sem1modconfpar:configuration:measure:refinput:n"] = setRefInputCount;
    m_ConfigXMLMap["sem1modconfpar:configuration:measure:activeunits:n"] = setActiveUnitCount;
    m_ConfigXMLMap["sem1modconfpar:configuration:measure:reactiveunits:n"] = setReactiveUnitCount;
    m_ConfigXMLMap["sem1modconfpar:configuration:measure:apparentunits:n"] = setApparentUnitCount;

    m_ConfigXMLMap["sem1modconfpar:parameter:measure:refinput"] = setRefInput;
    m_ConfigXMLMap["sem1modconfpar:parameter:measure:targeted"] = setTargeted;
    m_ConfigXMLMap["sem1modconfpar:parameter:measure:meastime"] = setMeasTime;
    m_ConfigXMLMap["sem1modconfpar:parameter:measure:upperlimit"] = setUpperLimit;
    m_ConfigXMLMap["sem1modconfpar:parameter:measure:lowerlimit"] = setLowerLimit;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}


QByteArray cSem1ModuleConfiguration::exportConfiguration()
{
    stringParameter* sPar;
    sPar = &m_pSem1ModulConfigData->m_sRefInput;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    boolParameter* bPar;
    bPar = &m_pSem1ModulConfigData->m_bTargeted;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));

    intParameter* iPar;
    iPar = &m_pSem1ModulConfigData->m_nMeasTime;
    m_pXMLReader->setValue(iPar->m_sKey, QString("%1").arg(iPar->m_nPar));

    doubleParameter* dPar;
    dPar = &m_pSem1ModulConfigData->m_fUpperLimit;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    dPar = &m_pSem1ModulConfigData->m_fLowerLimit;
    m_pXMLReader->setValue(dPar->m_sKey, QString("%1").arg(dPar->m_fPar));

    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSem1ModuleConfigData *cSem1ModuleConfiguration::getConfigurationData()
{
    return m_pSem1ModulConfigData;
}


void cSem1ModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setRefInputCount:
            m_pSem1ModulConfigData->m_nRefInpCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSem1ModulConfigData->m_nRefInpCount; i++) {
                m_ConfigXMLMap[QString("sem1modconfpar:configuration:measure:refinput:inp%1").arg(i+1)] = setRefInput1Name+i;
                m_ConfigXMLMap[QString("sem1modconfpar:configuration:measure:refinput_appends:append%1").arg(i+1)] = setRefInput1Append+i;
                m_pSem1ModulConfigData->m_refInpList.append(TRefInput());
            }
            break;
        case setActiveUnitCount:
            m_pSem1ModulConfigData->m_nActiveUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSem1ModulConfigData->m_nActiveUnitCount; i++) {
                m_ConfigXMLMap[QString("sem1modconfpar:configuration:measure:activeunits:unit%1").arg(i+1)] = setActiveUnit1Name+i;
                m_pSem1ModulConfigData->m_ActiveUnitList.append(QString());
            }
            break;
        case setReactiveUnitCount:
            m_pSem1ModulConfigData->m_nReactiveUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSem1ModulConfigData->m_nReactiveUnitCount; i++) {
                m_ConfigXMLMap[QString("sem1modconfpar:configuration:measure:reactiveunits:unit%1").arg(i+1)] = setReactiveUnit1Name+i;
                m_pSem1ModulConfigData->m_ReactiveUnitList.append(QString());
            }
            break;
        case setApparentUnitCount:
            m_pSem1ModulConfigData->m_nApparentUnitCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_pSem1ModulConfigData->m_nApparentUnitCount; i++) {
                m_ConfigXMLMap[QString("sem1modconfpar:configuration:measure:apparentunits:unit%1").arg(i+1)] = setApparentUnit1Name+i;
                m_pSem1ModulConfigData->m_ApparentUnitList.append(QString());
            }
            break;
        case setRefInput:
            m_pSem1ModulConfigData->m_sRefInput.m_sKey = key;
            m_pSem1ModulConfigData->m_sRefInput.m_sPar = m_pXMLReader->getValue(key);
            break;
        case setTargeted:
            m_pSem1ModulConfigData->m_bTargeted.m_sKey = key;
            m_pSem1ModulConfigData->m_bTargeted.m_nActive = m_pXMLReader->getValue(key).toInt();
            break;
        case setMeasTime:
            m_pSem1ModulConfigData->m_nMeasTime.m_sKey = key;
            m_pSem1ModulConfigData->m_nMeasTime.m_nPar = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setUpperLimit:
            m_pSem1ModulConfigData->m_fUpperLimit.m_sKey = key;
            m_pSem1ModulConfigData->m_fUpperLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;
        case setLowerLimit:
            m_pSem1ModulConfigData->m_fLowerLimit.m_sKey = key;
            m_pSem1ModulConfigData->m_fLowerLimit.m_fPar = m_pXMLReader->getValue(key).toDouble(&ok);
            break;

        default:
            if ((cmd >= setRefInput1Name) && (cmd < setRefInput1Name + 32)) {
                cmd -= setRefInput1Name;
                TRefInput refInput;
                refInput.inputName = m_pXMLReader->getValue(key);
                m_pSem1ModulConfigData->m_refInpList.replace(cmd, refInput);
            }
            else if ((cmd >= setActiveUnit1Name) && (cmd < setActiveUnit1Name + 16)) {
                cmd -= setActiveUnit1Name;
                QString name = m_pXMLReader->getValue(key);
                m_pSem1ModulConfigData->m_ActiveUnitList.replace(cmd, name);
            }
            else if ((cmd >= setReactiveUnit1Name) && (cmd < setReactiveUnit1Name + 16)) {
                cmd -= setReactiveUnit1Name;
                QString name = m_pXMLReader->getValue(key);
                m_pSem1ModulConfigData->m_ReactiveUnitList.replace(cmd, name);
            }
            else if ((cmd >= setApparentUnit1Name) && (cmd < setApparentUnit1Name + 16)) {
                cmd -= setApparentUnit1Name;
                QString name = m_pXMLReader->getValue(key);
                m_pSem1ModulConfigData->m_ApparentUnitList.replace(cmd, name);
            }
            else if ((cmd >= setRefInput1Append) && (cmd < setRefInput1Append + 32)) {
                cmd -= setRefInput1Append;
                TRefInput refInput = m_pSem1ModulConfigData->m_refInpList[cmd];
                refInput.nameAppend = m_pXMLReader->getValue(key);
                m_pSem1ModulConfigData->m_refInpList.replace(cmd, refInput);
            }
            break;
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}


void cSem1ModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

