#ifndef RECORDERMODULECONFIGDATA_H
#define RECORDERMODULECONFIGDATA_H

#include <QList>

struct entityconfiguration
{
    int m_entityId = 0;
    QStringList m_componentNames;
};


class RecorderModuleConfigData
{
public:
    quint8 m_entityCount;
    QList<entityconfiguration> m_entityConfigList;
};

#endif // RECORDERMODULECONFIGDATA_H
