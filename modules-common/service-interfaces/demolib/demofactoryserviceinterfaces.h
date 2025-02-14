#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRangeProg(QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceRangeObser(QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceRangeAdj(QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceDft(QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfacePower1(MeasModeSelector *measMode) override;
    DspInterfacePtr createDspInterfacePower2(MeasModeSelector* measMode) override;
    DspInterfacePtr createDspInterfaceSample(QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceThdn(QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOsci(QStringList valueChannelList, int interpolation) override;
    DspInterfacePtr createDspInterfaceMode() override;
    DspInterfacePtr createDspInterfaceRefProg(QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceRefAdj() override;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
