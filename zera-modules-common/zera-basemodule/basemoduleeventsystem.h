#ifndef BASEMODULEEVENTSYSTEM
#define BASEMODULEEVENTSYSTEM

#include "veinmodulecomponentinput.h"
#include <veincommandfiltereventsystem.h>
#include <QList>

class cBaseModuleEventSystem : public VeinCommandFilterEventSystem
{
    Q_OBJECT
public:
    cBaseModuleEventSystem();
    void setInputList(QList<cVeinModuleComponentInput*>& inputlist);
private:
    void processCommandEvent(VeinEvent::CommandEvent *commandEvent) override;
    QList<cVeinModuleComponentInput*> m_InputList;
};

#endif // BASEMODULEEVENTSYSTEM

