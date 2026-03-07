#ifndef RECORDERMODULECONFIGDATA_H
#define RECORDERMODULECONFIGDATA_H

#include <QVector>

struct ComponentConfiguration
{
    QString m_componentName;
    QString m_label;
};

struct EntityConfiguration
{
    int m_entityId = 0;
    QVector<ComponentConfiguration> m_components;
};


class RecorderModuleConfigData
{
public:
    int m_maxRecordingSeconds;
    quint8 m_entityCount;
    QVector<EntityConfiguration> m_entityConfigList;
    quint8 m_stackCount;
    QVector<quint8> m_valueLengthVector;
};

#endif // RECORDERMODULECONFIGDATA_H
