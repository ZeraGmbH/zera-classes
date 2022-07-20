#ifndef BASEMODULEEVENTSYSTEM
#define BASEMODULEEVENTSYSTEM

#include "veinmodulecomponentinput.h"
#include <ve_eventsystem.h>
#include <ve_commandevent.h>
#include <QList>

class cBaseModuleEventSystem: public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    cBaseModuleEventSystem(){}
    // EventSystem interface
    bool processEvent(QEvent *t_event) override final;
    void setInputList(QList<cVeinModuleComponentInput*>& inputlist);
private:
    virtual void processCommandEvent(VeinEvent::CommandEvent *t_cEvent);
    QList<cVeinModuleComponentInput*> m_InputList;
};

#endif // BASEMODULEEVENTSYSTEM

