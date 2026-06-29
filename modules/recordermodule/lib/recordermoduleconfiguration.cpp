#include "recordermoduleconfiguration.h"
#include "recordermoduleconfigdata.h"

RecorderModuleConfiguration::RecorderModuleConfiguration(const QByteArray &xmlString)
{
    setConfiguration(xmlString);
}

enum moduleconfigstate
{
    setMaxRecodingSeconds,
    setEntityCount,
    setInterpolationStack,

    setValueLen,
    setEntityId1 = 16, // we leave some place for additional cmds

    setComponentName1 = setEntityId1 + 10,
    setComponentLabel1 = setComponentName1 + 10,
    setnext = setComponentLabel1 + 10
};

void RecorderModuleConfiguration::setConfiguration(const QByteArray& xmlString)
{
    m_ConfigXMLMap["recordermoduleconfpar:configuration:maxrecording-seconds"] = setMaxRecodingSeconds;
    m_ConfigXMLMap["recordermoduleconfpar:configuration:entity-components:n"] = setEntityCount;
    m_ConfigXMLMap["recordermoduleconfpar:configuration:interpolation-stack:n"] = setInterpolationStack;

    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &RecorderModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &RecorderModuleConfiguration::completeConfiguration);
    m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
}

QByteArray RecorderModuleConfiguration::exportConfiguration() const
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}

RecorderModuleConfigData *RecorderModuleConfiguration::getConfigData()
{
    return &m_configData;
}

void RecorderModuleConfiguration::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        bool ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setMaxRecodingSeconds:
            m_configData.m_maxRecordingSeconds = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setEntityCount:
            m_configData.m_entityCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_configData.m_entityCount; i++) {
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:entity-components:ent%1:entity-id").arg(i+1)] = setEntityId1+i;
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:entity-components:ent%1:components").arg(i+1)] = setComponentName1+i;
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:entity-components:ent%1:labels").arg(i+1)] = setComponentLabel1+i;
            }
            m_configData.m_entityConfigList = QVector<EntityConfiguration>(m_configData.m_entityCount);
            break;
        case setInterpolationStack:
            m_configData.m_stackCount = m_pXMLReader->getValue(key).toInt(&ok);
            for (int i = 0; i < m_configData.m_stackCount; i++) {
                m_ConfigXMLMap[QString("recordermoduleconfpar:configuration:interpolation-stack:valuelen%1").arg(i+1)] = setValueLen+i;
            }
            m_configData.m_valueLengthVector = QVector<quint8>(m_configData.m_stackCount);
            break;
        default:
            // here we decode the dyn. generated cmd's
            if(cmd >=setValueLen && cmd <setEntityId1) {
                int index = cmd - setValueLen;
                m_configData.m_valueLengthVector[index] = m_pXMLReader->getValue(key).toInt(&ok);
            }

            else if (cmd >= setEntityId1 && cmd < setComponentName1) {
                cmd -= setEntityId1;
                int entityId = m_pXMLReader->getValue(key).toInt(&ok);
                m_configData.m_entityConfigList[cmd].m_entityId = entityId;
            }
            else if (cmd >= setComponentName1 && cmd < setComponentLabel1) {
                cmd -= setComponentName1;
                const QStringList componentNames = m_pXMLReader->getValue(key).split(",");
                m_configData.m_entityConfigList[cmd].m_components = QVector<ComponentConfiguration>(componentNames.count());
                for (int i=0; i<componentNames.count(); ++i)
                    m_configData.m_entityConfigList[cmd].m_components[i].m_componentName = componentNames[i];
            }
            else if (cmd >= setComponentLabel1 && cmd < setnext) {
                cmd -= setComponentLabel1;
                const QStringList label = m_pXMLReader->getValue(key).split(",");
                for (int i=0; i<label.count(); ++i)
                    m_configData.m_entityConfigList[cmd].m_components[i].m_label = label[i];
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
