#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>

#include <vcmp_componentdata.h>

namespace VeinEvent
{
    class EventSystem;
}


class cVeinModuleComponent: public QObject
{
    Q_OBJECT
public:
    cVeinModuleComponent(int entityId, VeinEvent::EventSystem* eventsystem, QString name, QString description, QVariant initval);

    virtual void exportMetaData(QJsonArray &jsArr);
    void setChannelName(QString name); // channel name for json export can be empty
    void setChannelUnit(QString unit);

public slots:
    void setValue(QVariant value); // here we have to emit event for notification

protected:
    int m_nEntityId;
    VeinEvent::EventSystem* m_pEventSystem;
    QString m_sName;
    QString m_sDescription;
    QVariant m_vValue;
    QString m_sChannelName;
    QString m_sChannelUnit;

    void exportComponentMetaData(QJsonObject jsObj);

private:
    void sendNotification(VeinComponent::ComponentData::Command vcmd);
};

#endif // VEINMODULECOMPONENT_H
