#ifndef SCPIEVENTSYSTEM
#define SCPIEVENTSYSTEM

#include <ve_eventsystem.h>

namespace VeinEvent
{
  class CommandEvent;
}

namespace SCPIMODULE
{

class cSCPIModule;

class cSCPIEventSystem: public VeinEvent::EventSystem
{
    Q_OBJECT

public:
    cSCPIEventSystem(cSCPIModule* module);
    // EventSystem interface
    bool processEvent(QEvent *t_event) override final;

signals:
    void status(quint8);
    void clientinfoSignal(QString);

private:
    cSCPIModule* m_pModule;
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
};
}

#endif // SCPIEVENTSYSTEM

