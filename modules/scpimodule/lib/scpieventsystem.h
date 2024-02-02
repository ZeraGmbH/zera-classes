#ifndef SCPIEVENTSYSTEM
#define SCPIEVENTSYSTEM

#include <vfeventsystemcommandfilter.h>

namespace VeinEvent
{
  class CommandEvent;
}

namespace SCPIMODULE
{

class cSCPIModule;

class cSCPIEventSystem: public VfEventSystemCommandFilter
{
    Q_OBJECT

public:
    cSCPIEventSystem(cSCPIModule* module);
signals:
    void status(quint8);
    void clientinfoSignal(QString);

private:
    cSCPIModule* m_pModule;
    void processCommandEvent(VeinEvent::CommandEvent *commandEvent) override;
};
}

#endif // SCPIEVENTSYSTEM

