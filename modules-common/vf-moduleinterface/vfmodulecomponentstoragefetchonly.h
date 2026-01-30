#ifndef VFMODULECOMPONENTSTORAGEFETCHONLY_H
#define VFMODULECOMPONENTSTORAGEFETCHONLY_H

#include "abstractmetascpicomponent.h"
#include <vfmodulemetainfocontainer.h>
#include <vs_abstractdatabase.h>
#include <QVariant>
#include <memory>

class VfModuleComponentStorageFetchOnly : public AbstractMetaScpiComponent
{
    Q_OBJECT
public:
    VfModuleComponentStorageFetchOnly(int entityId,
                                      const QString &componentName,
                                      const QString &description,
                                      VeinStorage::AbstractDatabase* storageDb);
    void setScpiInfo(const QString &model, const QString &cmd);
    void setStorageGetCustomizer(AbstractComponentGetCustomizerPtr getCustomizer);
    const VeinStorage::AbstractComponentPtr getStorageComponent();

    void exportMetaData(QJsonObject &jsObj) override;
    void exportSCPIInfo(QJsonArray &jsArr) override;

private:
    QString m_componentName;
    QString m_description;
    std::unique_ptr<VfModuleMetaInfoContainer> m_scpiInfo;
    const VeinStorage::AbstractComponentPtr m_storageComponent;
};

#endif // VFMODULECOMPONENTSTORAGEFETCHONLY_H
