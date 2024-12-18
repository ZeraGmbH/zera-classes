#ifndef ABSTRACTFACTORYSERVICEINTERFACES_H
#define ABSTRACTFACTORYSERVICEINTERFACES_H

#include <measmodeselector.h>
#include <dspinterface.h>
#include <memory>

typedef std::shared_ptr<Zera::cDSPInterface> DspInterfacePtr;

class AbstractFactoryServiceInterfaces
{
public:
    virtual DspInterfacePtr createDspInterfaceRange(QStringList valueChannelList, bool isReferencce) = 0;
    virtual DspInterfacePtr createDspInterfaceDft(QStringList valueChannelList, int dftOrder) = 0;
    virtual DspInterfacePtr createDspInterfaceFft(QStringList valueChannelList, int fftOrder) = 0;
    virtual DspInterfacePtr createDspInterfaceRms(QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfacePower1(MeasModeSelector* measMode) = 0;
    virtual DspInterfacePtr createDspInterfacePower2(MeasModeSelector* measMode) = 0;
    virtual DspInterfacePtr createDspInterfaceSample(QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceThdn(QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceOsci(QStringList valueChannelList, int interpolation) = 0;
    virtual DspInterfacePtr createDspInterfaceMode() = 0;
    virtual DspInterfacePtr createDspInterfaceRef(QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceOther() = 0;
};

typedef std::shared_ptr<AbstractFactoryServiceInterfaces> AbstractFactoryServiceInterfacesPtr;

#endif // ABSTRACTFACTORYSERVICEINTERFACES_H
