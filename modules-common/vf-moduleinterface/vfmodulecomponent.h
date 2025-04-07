#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include <scpiveincomponentinfo.h>
#include <ve_eventsystem.h>
#include <vcmp_componentdata.h>

class VfModuleComponent: public QObject
{
    Q_OBJECT
public:
    VfModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval);
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
    void exportSCPIInfo(QJsonArray &jsArr);
signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values
    void sigValueQuery(QVariant); // we connect here if we want to read a value before returning data from storage ...perhaps with parameter
public slots:
    void setValue(QVariant value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification

protected:
    void sendNotification(VeinComponent::ComponentData::Command vcmd);
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QString m_sDescription;
    QVariant m_vValue;
    QString m_sChannelName;
    QString m_sChannelUnit;
    QList<QUuid> mClientIdList;
private:
    int m_nEntityId;
    std::unique_ptr<ScpiVeinComponentInfo> m_scpiInfo;
};

#endif // VEINMODULECOMPONENT_H
