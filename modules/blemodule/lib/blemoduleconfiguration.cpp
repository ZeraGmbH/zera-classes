#include "blemoduleconfiguration.h"
#include <xmlconfigreader.h>
#include <QString>

namespace BLEMODULE
{
cBleModuleConfiguration::cBleModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cBleModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cBleModuleConfiguration::completeConfiguration);
}

cBleModuleConfiguration::~cBleModuleConfiguration()
{
    delete m_pBleModulConfigData;
}

void cBleModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pBleModulConfigData)
        delete m_pBleModulConfigData;
    m_pBleModulConfigData = new cBleModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration
    m_ConfigXMLMap["blemodconfpar:parameter:bluetoothon"] = setBluetoothOn;
    m_ConfigXMLMap["blemodconfpar:parameter:macaddress"] = setMacAddress;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cBleModuleConfiguration::exportConfiguration()
{
    boolParameter *bPar = &m_pBleModulConfigData->m_bluetoothOn;
    m_pXMLReader->setValue(bPar->m_sKey, QString("%1").arg(bPar->m_nActive));
    stringParameter *sPar = &m_pBleModulConfigData->m_macAddress;
    m_pXMLReader->setValue(sPar->m_sKey, sPar->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cBleModuleConfigData *cBleModuleConfiguration::getConfigurationData()
{
    return m_pBleModulConfigData;
}

void cBleModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setBluetoothOn:
            m_pBleModulConfigData->m_bluetoothOn.m_sKey = key;
            m_pBleModulConfigData->m_bluetoothOn.m_nActive = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMacAddress:
            m_pBleModulConfigData->m_macAddress.m_sKey = key;
            m_pBleModulConfigData->m_macAddress.m_sPar = m_pXMLReader->getValue(key);
            break;
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void cBleModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
