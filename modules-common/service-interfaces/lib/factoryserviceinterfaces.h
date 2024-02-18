#ifndef FACTORYSERVICEINTERFACES_H
#define FACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class FactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr getDspInterfaceOther() override;
};

#endif // FACTORYSERVICEINTERFACES_H
