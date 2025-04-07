#include "vfmoduleactvalue.h"

VfModuleActvalue::VfModuleActvalue(int entityId,
                                   VeinEvent::EventSystem *eventsystem,
                                   QString name,
                                   QString description,
                                   QVariant initval) :
    VfModuleComponent(entityId, eventsystem, name, description, initval)
{
}
