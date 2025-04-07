#ifndef VEINMODULEACTVALUE_H
#define VEINMODULEACTVALUE_H

#include "vfmodulecomponent.h"

class VfModuleActvalue: public VfModuleComponent
{
    Q_OBJECT
public:
    VfModuleActvalue(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval = QVariant());
private:
};


#endif // VEINMODULEACTVALUE_H
