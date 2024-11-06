#ifndef SCPIEVENTSYSTEM
#define SCPIEVENTSYSTEM

#include "scpimodule.h"
#include <vfeventsystemcommandfilter.h>
#include <ve_commandevent.h>

namespace SCPIMODULE
{

class SCPIEventSystem: public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    SCPIEventSystem(cSCPIModule* module);
signals:
    void sigClientInfoSignal(QString);

private:
    cSCPIModule* m_pModule;
    void processCommandEvent(VeinEvent::CommandEvent *commandEvent) override;
    void handleComponentData(VeinEvent::CommandEvent *commandEvent);
    void handleErrorData(VeinEvent::CommandEvent *commandEvent);
};

}

#endif // SCPIEVENTSYSTEM

