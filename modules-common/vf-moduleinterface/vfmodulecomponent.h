#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include "abstractmetascpicomponent.h"
#include "vfmodulemetainfocontainer.h"
#include <ve_eventsystem.h>
#include <vcmp_componentdata.h>

class VfModuleComponent: public AbstractMetaScpiComponent
{
    Q_OBJECT
public:
    VfModuleComponent(int entityId,
                      VeinEvent::EventSystem *eventsystem,
                      const QString &componentName,
                      const QString &description,
                      const QVariant &initval = QVariant());

    void setChannelName(QString name); // channel name for json export can be empty
    QString getChannelName();
    void setUnit(QString unit);
    QVariant getValue();
    QString getUnit();
    QString getComponentName();
    void exportMetaData(QJsonObject &jsObj) override;

    void setScpiInfo(const QString &model, const QString &cmd,
                     int cmdTypeMask, // e.g SCPI::isQuery|SCPI::isCmdwP
                     SCPI::eSCPIEntryType entryType = SCPI::isComponent);
    void exportSCPIInfo(QJsonArray &jsArr) override;
    void sendDummyNotificationForRangeChange();

signals:
    void sigValueChanged(const QVariant &value); // we connect here if we want to do something on changed values
public slots:
    void setValue(const QVariant &value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification

protected:
    QString m_componentName;
    QString m_sDescription;
    QVariant m_vValue;
    QString m_sChannelName;
    QString m_sChannelUnit;
    QList<QUuid> m_clientIdList;

private:
    void sendNotification(VeinComponent::ComponentData::Command vcmd);

    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    std::unique_ptr<VfModuleMetaInfoContainer> m_scpiInfo;
};

#endif // VEINMODULECOMPONENT_H
