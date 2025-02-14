#ifndef ABSTRACTFACTORYSERVICEINTERFACES_H
#define ABSTRACTFACTORYSERVICEINTERFACES_H

#include <measmodeselector.h>
#include <dspinterface.h>
#include <memory>

typedef std::shared_ptr<Zera::cDSPInterface> DspInterfacePtr;

class AbstractFactoryServiceInterfaces
{
public:
    virtual DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) = 0;
    virtual DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) = 0;
    virtual DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) = 0;
    virtual DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) = 0;
    virtual DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) = 0;
    virtual DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) = 0;
    virtual DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) = 0;
    virtual DspInterfacePtr createDspInterfaceSample(int entityId, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) = 0;
    virtual DspInterfacePtr createDspInterfaceMode(int entityId) = 0;
    virtual DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceRefAdj(int entityId) = 0;
};

typedef std::shared_ptr<AbstractFactoryServiceInterfaces> AbstractFactoryServiceInterfacesPtr;

#endif // ABSTRACTFACTORYSERVICEINTERFACES_H
