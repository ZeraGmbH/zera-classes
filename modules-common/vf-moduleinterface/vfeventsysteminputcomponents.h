#ifndef VFEVENTSYSTEMINPUTCOMPONENTS_H
#define VFEVENTSYSTEMINPUTCOMPONENTS_H

#include "vfeventsystemcommandfilter.h"
#include "vfmodulecomponentinput.h"
#include <QList>

class VfEventSystemInputComponents : public VfEventSystemCommandFilter
{
    Q_OBJECT
public:
    VfEventSystemInputComponents();
    void setInputList(QList<VfModuleComponentInput*>& inputComponentList);
private:
    void processCommandEvent(VeinEvent::CommandEvent *commandEvent) override;
    QList<VfModuleComponentInput*> m_inputComponentList;
};

#endif // VFEVENTSYSTEMINPUTCOMPONENTS_H
