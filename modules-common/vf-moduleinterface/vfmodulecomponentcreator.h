#ifndef VFMODULECOMPONENTCREATOR_H
#define VFMODULECOMPONENTCREATOR_H

#include "vfmodulecomponent.h"
#include <ve_eventsystem.h>

class VfModuleComponentCreator
{
public:
    VfModuleComponentCreator(int entityId,
                             VeinEvent::EventSystem *eventsystem);
    VfModuleComponent* createComponent(const QString &componentName, const QString &description,
                                       const QString &channelName = "", const QString &unit = "",
                                       const QString &scpiModel = "", const QString &scpiCmd = "", int scpiCmdTypeMask = 0,
                                       const QVariant &initval = QVariant()) const;

private:
    int m_entityId = 0;
    VeinEvent::EventSystem *m_eventsystem = nullptr;
};

#endif // VFMODULECOMPONENTCREATOR_H
