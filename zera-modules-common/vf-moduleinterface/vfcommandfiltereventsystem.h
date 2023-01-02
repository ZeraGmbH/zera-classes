#ifndef VEINCOMMANDFILTEREVENTSYSTEM_H
#define VEINCOMMANDFILTEREVENTSYSTEM_H

#include <ve_eventsystem.h>
#include <ve_commandevent.h>

class VfCommandFilterEventSystem : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    VfCommandFilterEventSystem(VeinEvent::CommandEvent::EventSubtype subtypeToFilter);
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent) = 0;
    bool processEvent(QEvent *event) override final;
private:
    VeinEvent::CommandEvent::EventSubtype m_subtypeToFilter;
};

#endif // VEINCOMMANDFILTEREVENTSYSTEM_H
