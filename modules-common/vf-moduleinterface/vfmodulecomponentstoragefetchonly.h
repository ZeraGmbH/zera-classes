#ifndef VFMODULECOMPONENTSTORAGEFETCHONLY_H
#define VFMODULECOMPONENTSTORAGEFETCHONLY_H

#include "abstractmetascpicomponent.h"
#include <vfmodulemetainfocontainer.h>
#include <QVariant>
#include <memory>

class VfModuleComponentStorageFetchOnly : public AbstractMetaScpiComponent
{
    Q_OBJECT
public:
    VfModuleComponentStorageFetchOnly(int entityId,
                                      const QString &componentName,
                                      const QString &description);
    void setScpiInfo(const QString &model, const QString &cmd);
    void exportMetaData(QJsonObject &jsObj) override;
    void exportSCPIInfo(QJsonArray &jsArr) override;
private:
    int m_entityId;
    QString m_componentName;
    QString m_description;
    std::unique_ptr<VfModuleMetaInfoContainer> m_scpiInfo;
};

#endif // VFMODULECOMPONENTSTORAGEFETCHONLY_H
