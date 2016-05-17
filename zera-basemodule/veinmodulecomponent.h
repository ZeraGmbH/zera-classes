#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>

#include <vcmp_componentdata.h>

#include "metadata.h"

namespace VeinEvent
{
    class EventSystem;
}


class cVeinModuleComponent: public cMetaData
{
    Q_OBJECT
public:
    cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval);

    virtual void exportMetaData(QJsonObject &jsObj);
    void setChannelName(QString name); // channel name for json export can be empty
    QString getChannelName();
    void setUnit(QString unit);
    QVariant getValue();
    QString getUnit();
    QString getName();

signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values

public slots:
    void setValue(QVariant value); // here we have to emit event for notification

protected:
    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QString m_sDescription;
    QVariant m_vValue;
    QString m_sChannelName;
    QString m_sChannelUnit;

private:
    void sendNotification(VeinComponent::ComponentData::Command vcmd);
};

#endif // VEINMODULECOMPONENT_H
