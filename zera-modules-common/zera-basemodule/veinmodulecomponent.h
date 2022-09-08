#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include "metadata.h"
#include <ve_eventsystem.h>
#include <vcmp_componentdata.h>

class cVeinModuleComponent: public cMetaData
{
    Q_OBJECT
public:
    cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval);
    ~cVeinModuleComponent();
    virtual void exportMetaData(QJsonObject &jsObj);
    void setChannelName(QString name); // channel name for json export can be empty
    QString getChannelName();
    void setUnit(QString unit);
    const QVariant& getValue() const;
    QString getUnit();
    QString getName();
signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values
    void sigValueQuery(QVariant); // we connect here if we want to read a value before returning data from storage ...perhaps with parameter
public slots:
    void setValue(QVariant value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification
protected:
    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QString m_sDescription;
    QVariant m_vValue;
    QString m_sChannelName;
    QString m_sChannelUnit;
    QList<QUuid> mClientIdList;

protected:
    void sendNotification(VeinComponent::ComponentData::Command vcmd);
};

#endif // VEINMODULECOMPONENT_H
