#include "dosagemoduleconfiguration.h"

static void initResource()
{
    Q_INIT_RESOURCE(dosagemodulexmlschema);
}

namespace DOSAGEMODULE
{
enum moduleconfigstate
{
    setSystemCount,

    setMeasSystem1 = 10 // we leave some place for additional cmds
};


cDosageModuleConfiguration::cDosageModuleConfiguration(const QByteArray &xmlString)
{
    initResource();
    setConfiguration(xmlString);
}

void cDosageModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cDosageModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cDosageModuleConfiguration::completeConfiguration);

    m_ConfigXMLMap["dosagemodconfpar:configuration:measure:system:n"] = setSystemCount;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray cDosageModuleConfiguration::exportConfiguration() const
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

cDosageModuleConfigData *cDosageModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void cDosageModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
            case setSystemCount:
            {
                dosagesystemconfiguration dsc;
                dsc.m_ComponentName = "ACT_PQS0";  // is dummy

                m_configData.m_nDosageSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
                for (int i = 0; i < m_configData.m_nDosageSystemCount; i++)    // generate dynamic hash entries for value channel configuration
                {
                    m_ConfigXMLMap[QString("dosagemodconfpar:configuration:measure:system:dos%1").arg(i+1)] = setMeasSystem1+i;
                    m_configData.m_DosageSystemConfigList.append(dsc);
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
                    QStringList dosChannels = dosageSystem.split(",");  // currently: entity, component and limit
                    dosagesystemconfiguration dsc = m_configData.m_DosageSystemConfigList.at(cmd);
                    dsc.m_nEntity = dosChannels.at(0).toInt();
                    dsc.m_ComponentName = dosChannels.at(1);
                    dsc.m_fUpperLimit = dosChannels.at(2).toDouble();
                    m_configData.m_DosageSystemConfigList.replace(cmd, dsc);
                }
                else
                    qWarning("Error dosagemodule: cmd (%i) is out of range", cmd);
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
