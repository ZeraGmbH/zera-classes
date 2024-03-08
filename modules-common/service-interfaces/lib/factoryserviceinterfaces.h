#ifndef FACTORYSERVICEINTERFACES_H
#define FACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class FactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce) override;
    DspInterfacePtr createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfacePower1(int interruptNoHandled, MeasModeSelector* measMode) override;
    DspInterfacePtr createDspInterfaceOther() override;
};

#endif // FACTORYSERVICEINTERFACES_H
