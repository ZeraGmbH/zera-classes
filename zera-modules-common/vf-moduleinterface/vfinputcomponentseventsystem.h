#ifndef VEININPUTCOMPONENTSEVENTSYSTEM_H
#define VEININPUTCOMPONENTSEVENTSYSTEM_H

#include "vfcommandfiltereventsystem.h"
#include "vfmodulecomponentinput.h"
#include <QList>

class VfInputComponentsEventSystem : public VfCommandFilterEventSystem
{
    Q_OBJECT
public:
    VfInputComponentsEventSystem();
    void setInputList(QList<VfModuleComponentInput*>& inputComponentList);
private:
    void processCommandEvent(VeinEvent::CommandEvent *commandEvent) override;
    QList<VfModuleComponentInput*> m_inputComponentList;
};

#endif // VEININPUTCOMPONENTSEVENTSYSTEM_H
