#ifndef BASEMEASMODULE
#define BASEMEASMODULE

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>
#include <QByteArray>
#include <QTimer>
#include <virtualmodule.h>

#include "basemodule.h"


namespace VeinEvent
{
    class EventSystem;
    class StorageSystem;
}


class cModuleValidator;
class cVeinModuleComponent;

class cBaseMeasProgram;
class cBaseModuleConfiguration;
class cBaseMeasChannel;


class cBaseMeasModule : public cBaseModule
{
Q_OBJECT

public:
    cBaseMeasModule(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, QObject *parent = 0);

    cModuleValidator* m_pModuleValidator;
};

#endif // BASEMEASMODULE

