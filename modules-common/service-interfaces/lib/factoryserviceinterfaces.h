#ifndef FACTORYSERVICEINTERFACES_H
#define FACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class FactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOther() override;
};

#endif // FACTORYSERVICEINTERFACES_H
