#ifndef FACTORYSERVICEINTERFACES_H
#define FACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class FactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRange(QStringList valueChannelList, bool isReferencce) override;
    DspInterfacePtr createDspInterfaceDft(QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfacePower1(MeasModeSelector* measMode) override;
    DspInterfacePtr createDspInterfaceSample(QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOther() override;
};

#endif // FACTORYSERVICEINTERFACES_H
