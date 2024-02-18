#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr getDspInterfaceOther() override;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
