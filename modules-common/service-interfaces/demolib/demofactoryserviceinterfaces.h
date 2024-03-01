#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce) override;
    DspInterfacePtr createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOther() override;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
