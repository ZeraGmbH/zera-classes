#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOther() override;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
