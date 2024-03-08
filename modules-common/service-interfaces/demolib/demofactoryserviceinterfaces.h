#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce) override;
    DspInterfacePtr createDspInterfaceDft(QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOther() override;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
