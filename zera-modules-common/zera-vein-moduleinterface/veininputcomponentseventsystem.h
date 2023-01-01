#ifndef VEININPUTCOMPONENTSEVENTSYSTEM_H
#define VEININPUTCOMPONENTSEVENTSYSTEM_H

#include "veincommandfiltereventsystem.h"
#include "veinmodulecomponentinput.h"
#include <QList>

class VeinInputComponentsEventSystem : public VeinCommandFilterEventSystem
{
    Q_OBJECT
public:
    VeinInputComponentsEventSystem();
    void setInputList(QList<cVeinModuleComponentInput*>& inputComponentList);
private:
    void processCommandEvent(VeinEvent::CommandEvent *commandEvent) override;
    QList<cVeinModuleComponentInput*> m_inputComponentList;
};

#endif // VEININPUTCOMPONENTSEVENTSYSTEM_H
