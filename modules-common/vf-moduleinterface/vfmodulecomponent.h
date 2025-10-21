#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include <scpiveincomponentinfo.h>
#include <ve_eventsystem.h>
#include <vcmp_componentdata.h>

class VfModuleComponent: public QObject
{
    Q_OBJECT
public:
    VfModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval = QVariant());
    ~VfModuleComponent();

    void setChannelName(QString name); // channel name for json export can be empty
    QString getChannelName();
    void setUnit(QString unit);
    QVariant getValue();
    QString getUnit();
    QString getName();
    void exportMetaData(QJsonObject &jsObj);

    void setScpiInfo(const QString &model, const QString &cmd,
                     int cmdTypeMask, // e.g SCPI::isQuery|SCPI::isCmdwP
                     const QString &veinComponentName,
                     SCPI::eSCPIEntryType entryType = SCPI::isComponent);
    void setRPCScpiInfo(const QString &model, const QString &cmd,
                     int cmdTypeMask, // e.g SCPI::isQuery|SCPI::isCmdwP
                     const QString &veinComponentName,
                     const QString &rpcParamsType,
                     SCPI::eSCPIEntryType entryType = SCPI::isComponent);
    void exportSCPIInfo(QJsonArray &jsArr);
    void exportRpcSCPIInfo(QJsonArray &jsArr);
    static int getInstanceCount();
signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values
public slots:
    void setValue(QVariant value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification

protected:
    QString m_sName;
    QString m_sDescription;
    QVariant m_vValue;
    QString m_sChannelName;
    QString m_sChannelUnit;
    QList<QUuid> mClientIdList;

private:
    void sendNotification(VeinComponent::ComponentData::Command vcmd);

    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    std::unique_ptr<ScpiVeinComponentInfo> m_scpiInfo;
    std::unique_ptr<ScpiVeinComponentInfo> m_RpcScpiInfo;

    static int m_instanceCount;
};

#endif // VEINMODULECOMPONENT_H
