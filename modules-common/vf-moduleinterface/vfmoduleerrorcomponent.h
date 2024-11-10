#ifndef VEINMODULEERRORCOMPONENT
#define VEINMODULEERRORCOMPONENT

#include "vfmodulecomponent.h"

class VfModuleErrorComponent: public QObject
{
    Q_OBJECT
public:
    VfModuleErrorComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString modulename);
public slots:
    void setValue(QVariant value); // here we have to emit event for notification
protected:
    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QString m_sModuleName;
};

#endif // VEINMODULEERRORCOMPONENT

