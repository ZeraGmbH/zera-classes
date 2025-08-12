#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "dosagemoduleconfiguration.h"
#include "dosagemoduleconfigdata.h"


static void initResource()
{
    Q_INIT_RESOURCE(dosagemodulexmlschema);
}

namespace DOSAGEMODULE
{
static const char *defaultXSDFile = "://dosagemodule.xsd";

cDosageModuleConfiguration::cDosageModuleConfiguration()
{
    initResource();
    m_pDosageModulConfigData = nullptr;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cDosageModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cDosageModuleConfiguration::completeConfiguration);
}

cDosageModuleConfiguration::~cDosageModuleConfiguration()
{
    if (m_pDosageModulConfigData)
        delete m_pDosageModulConfigData;
}

void cDosageModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pDosageModulConfigData)
        delete m_pDosageModulConfigData;
    m_pDosageModulConfigData = new cDosageModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["dosagemodconfpar:configuration:measure:inputentity"] = setInputModule;
    m_ConfigXMLMap["dosagemodconfpar:configuration:measure:system:n"] = setSystemCount;

    // was is den 4 Power componenten???


    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));

    if (m_bConfigError)
        qWarning("m_bConfigError = true");
    else
        qWarning("m_bConfigError = false");
    if (m_bConfigured)
        qWarning("m_bConfigured = true");
    else
        qWarning("m_bConfigured = false");
}

QByteArray cDosageModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cDosageModuleConfigData *cDosageModuleConfiguration::getConfigurationData()
{
    return m_pDosageModulConfigData;
}


void cDosageModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
            case setInputModule:
                m_pDosageModulConfigData->m_nModuleId = m_pXMLReader->getValue(key).toInt(&ok);
                break;
            case setSystemCount:
            {
                dosagesystemconfiguration dsc;
                dsc.m_ComponentName = "ACT_PQS0";  // is dummy

                m_pDosageModulConfigData->m_nDosageSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
                for (int i = 0; i < m_pDosageModulConfigData->m_nDosageSystemCount; i++)    // generate dynamic hash entries for value channel configuration
                {
                    m_ConfigXMLMap[QString("dosagemodconfpar:configuration:measure:system:dos%1").arg(i+1)] = setMeasSystem1+i;
                    m_pDosageModulConfigData->m_DosageSystemConfigList.append(dsc);
                }
                break;
            }
            default:

                // here we decode the dyn. generated cmd's
                if ((cmd >= setMeasSystem1) && (cmd < (setMeasSystem1 * 2)))
                {
                    cmd -= setMeasSystem1;
                    // it is command for setting measuring mode
                    QString dosageSystem = m_pXMLReader->getValue(key);
                    QStringList dosChannels = dosageSystem.split(",");  // currently component and limit
                    dosagesystemconfiguration dsc = m_pDosageModulConfigData->m_DosageSystemConfigList.at(cmd);
                    dsc.m_ComponentName = dosChannels.at(0);
                    dsc.m_fUpperLimit = dosChannels.at(1).toDouble();
                    m_pDosageModulConfigData->m_DosageSystemConfigList.replace(cmd, dsc);
                }
                else
                    qWarning("Error DosageModule: cmd is out of range");
        }

        if (!m_bConfigError)        // replaces: m_bConfigError |= !ok;
            m_bConfigError = !ok;
    }
    else
    {
        m_bConfigError = true;
        qWarning("cmd: %s has no case", qPrintable(key));
    }
}

void cDosageModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}
