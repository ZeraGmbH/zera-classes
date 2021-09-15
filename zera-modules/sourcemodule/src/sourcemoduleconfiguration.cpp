#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "sourcemoduleconfiguration.h"
#include "sourcemoduleconfigdata.h"
#include "socket.h"

namespace SOURCEMODULE
{

cSourceModuleConfiguration::cSourceModuleConfiguration()
{
    m_pStatusModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cSourceModuleConfiguration::~cSourceModuleConfiguration()
{
    if (m_pStatusModulConfigData) delete m_pStatusModulConfigData;
}


void cSourceModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pStatusModulConfigData) delete m_pStatusModulConfigData;
    m_pStatusModulConfigData = new cSourceModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["statusmodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cSourceModuleConfiguration::exportConfiguration()
{
    // nothing to set befor export
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cSourceModuleConfigData *cSourceModuleConfiguration::getConfigurationData()
{
    return m_pStatusModulConfigData;
}


void cSourceModuleConfiguration::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        int cmd = m_ConfigXMLMap[key];
        bool valueOK = false;
        switch (cmd)
        {
        case setDebugLevel:
            m_pStatusModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&valueOK);
            break;
        }
        m_bConfigError |= !valueOK;
    }
    else {
        m_bConfigError = true;
    }
}


void cSourceModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

