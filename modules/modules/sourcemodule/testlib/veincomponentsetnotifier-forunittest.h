#ifndef VENCOMPONENTNOTIFIER_H
#define VENCOMPONENTNOTIFIER_H

#include <vfmodulecomponent.h>
#include <ve_eventsystem.h>
#include <ve_eventhandler.h>
#include <vs_veinhash.h>

#include <QObject>
#include <QHash>
#include <QVariant>

class VeinComponentSetNotifier : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    VeinComponentSetNotifier(int entityId);
    void addComponentToNotify(QString componentName, VfModuleComponent* component);
signals:
    void sigComponentChanged(QString componentName, QVariant newValue);
private:
    void createEntity(int entityId);
    void processEvent(QEvent *t_event) override;

    VeinEvent::EventHandler m_eventHandler;
    VeinStorage::VeinHash m_storageHash;
    QHash <QString, VfModuleComponent*> m_hashComponentsListening;
};

#endif // VENCOMPONENTNOTIFIER_H
