#ifndef ABSTRACTFACTORYSERVICEINTERFACES_H
#define ABSTRACTFACTORYSERVICEINTERFACES_H

#include <measmodeselector.h>
#include <dspinterface.h>
#include <memory>

class AbstractFactoryServiceInterfaces
{
public:
    virtual void resetInterfaces() {}
    virtual Zera::DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) = 0;
    virtual Zera::DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) = 0;
    virtual Zera::DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) = 0;
    virtual Zera::DspInterfacePtr createDspInterfacePeriodAverage(int entityId, QStringList valueChannelList, int maxPeriodCount, int initialPeriodCount) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceMode(int entityId) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) = 0;
    virtual Zera::DspInterfacePtr createDspInterfaceRefAdj(int entityId) = 0;
};

typedef std::shared_ptr<AbstractFactoryServiceInterfaces> AbstractFactoryServiceInterfacesPtr;

#endif // ABSTRACTFACTORYSERVICEINTERFACES_H
