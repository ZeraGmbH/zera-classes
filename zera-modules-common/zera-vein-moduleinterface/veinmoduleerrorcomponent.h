#ifndef VEINMODULEERRORCOMPONENT
#define VEINMODULEERRORCOMPONENT

#include "veinmodulecomponent.h"

enum destType
{
    localDest,
    globalDest
};

class cVeinModuleErrorComponent: public QObject
{
    Q_OBJECT
public:
    cVeinModuleErrorComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString modulename);
public slots:
    void setValue(QVariant value, int dest = globalDest); // here we have to emit event for notification
protected:
    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QString m_sModuleName;
};

#endif // VEINMODULEERRORCOMPONENT

