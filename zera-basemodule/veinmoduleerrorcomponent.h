#ifndef VEINMODULEERRORCOMPONENT
#define VEINMODULEERRORCOMPONENT

#include <QObject>

#include "veinmodulecomponent.h"

class cVeinModuleErrorComponent: public QObject
{
    Q_OBJECT
public:
    cVeinModuleErrorComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString modulename);

public slots:
    void setValue(QVariant value); // here we have to emit event for notification

protected:
    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QString m_sModuleName;
};

#endif // VEINMODULEERRORCOMPONENT

