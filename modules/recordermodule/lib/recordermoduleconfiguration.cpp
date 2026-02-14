#include "recordermoduleconfiguration.h"
#include "recordermoduleconfigdata.h"


enum moduleconfigstate
{
    setEntityCount,

    setEntityId1 = 16, // we leave some place for additional cmds

    setComponentName1 = setEntityId1 + 10,
    setComponentLabel1 = setComponentName1 + 10,
    setnext = setComponentLabel1 + 10
};


RecorderModuleConfiguration::RecorderModuleConfiguration()
{
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &RecorderModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &RecorderModuleConfiguration::completeConfiguration);
}

RecorderModuleConfiguration::~RecorderModuleConfiguration()
{
    if(m_pRecorderModuleConfigData)
        delete m_pRecorderModuleConfigData;
}

void RecorderModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_bConfigured = m_bConfigError = false;
    if (m_pRecorderModuleConfigData)
        delete m_pRecorderModuleConfigData;
    m_pRecorderModuleConfigData = new RecorderModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration
    m_ConfigXMLMap["recordermoduleconfpar:configuration:entity-components:n"] = setEntityCount;

    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray RecorderModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

RecorderModuleConfigData *RecorderModuleConfiguration::getConfigurationData()
{
    return m_pRecorderModuleConfigData;
}

void RecorderModuleConfiguration::configXMLInfo(const QString &key)
{
    bool ok;
    if (m_ConfigXMLMap.contains(key)) {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setEntityCount:
            m_pRecorderModuleConfigData->m_entityCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pRecorderModuleConfigData->m_entityCount; i++) {
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:entity-components:ent%1:entity-id").arg(i+1)] = setEntityId1+i;
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:entity-components:ent%1:components").arg(i+1)] = setComponentName1+i;
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:entity-components:ent%1:labels").arg(i+1)] = setComponentLabel1+i;
            }
            m_pRecorderModuleConfigData->m_entityConfigList = QVector<EntityConfiguration>(m_pRecorderModuleConfigData->m_entityCount);
            break;
        default:
            // here we decode the dyn. generated cmd's
            if (cmd >= setEntityId1 && cmd < setComponentName1) {
                cmd -= setEntityId1;
                int entityId = m_pXMLReader->getValue(key).toInt(&ok);
                m_pRecorderModuleConfigData->m_entityConfigList[cmd].m_entityId = entityId;
            }
            else if (cmd >= setComponentName1 && cmd < setComponentLabel1) {
                cmd -= setComponentName1;
                const QStringList componentNames = m_pXMLReader->getValue(key).split(",");
                m_pRecorderModuleConfigData->m_entityConfigList[cmd].m_components = QVector<ComponentConfiguration>(componentNames.count());
                for (int i=0; i<componentNames.count(); ++i)
                    m_pRecorderModuleConfigData->m_entityConfigList[cmd].m_components[i].m_componentName = componentNames[i];
            }
            else if (cmd >= setComponentLabel1 && cmd < setnext) {
                cmd -= setComponentLabel1;
                const QStringList label = m_pXMLReader->getValue(key).split(",");
                for (int i=0; i<label.count(); ++i)
                    m_pRecorderModuleConfigData->m_entityConfigList[cmd].m_components[i].m_label = label[i];
            }
        }
        m_bConfigError |= !ok;
    }
    else
        m_bConfigError = true;
}

void RecorderModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}
