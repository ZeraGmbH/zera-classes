#include "blemoduleconfiguration.h"

namespace BLEMODULE
{

cBleModuleConfiguration::cBleModuleConfiguration(const QByteArray& xmlString)
{
    setConfiguration(xmlString);
}

void cBleModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["blemodconfpar:parameter:bluetoothon"] = setBluetoothOn;
    m_ConfigXMLMap["blemodconfpar:parameter:macaddress"] = setMacAddress;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cBleModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cBleModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cBleModuleConfiguration::exportConfiguration() const
{
    const boolParameter *paramBluetoothOn = &m_configData.m_bluetoothOn;
    m_pXMLReader->setValue(paramBluetoothOn->m_sKey, QString("%1").arg(paramBluetoothOn->m_nActive));

    const stringParameter *paramMachAddress = &m_configData.m_macAddress;
    m_pXMLReader->setValue(paramMachAddress->m_sKey, paramMachAddress->m_sPar);

    return m_pXMLReader->getXMLConfig().toUtf8();
}

cBleModuleConfigData *cBleModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cBleModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setBluetoothOn:
            m_configData.m_bluetoothOn.m_sKey = key;
            m_configData.m_bluetoothOn.m_nActive = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setMacAddress:
            m_configData.m_macAddress.m_sKey = key;
            m_configData.m_macAddress.m_sPar = m_pXMLReader->getValue(key);
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
