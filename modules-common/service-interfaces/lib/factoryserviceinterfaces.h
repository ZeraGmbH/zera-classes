#ifndef FACTORYSERVICEINTERFACES_H
#define FACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"

class FactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    Zera::DspInterfacePtr createDspInterfaceRangeProg(int entityId, const QStringList &valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeObser(int entityId, const QStringList &valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeAdj(int entityId, const QStringList &valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceDft(int entityId, const QStringList &valueChannelList, int dftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceFft(int entityId, const QStringList &valueChannelList, int fftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceRms(int entityId, const QStringList &valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfaceThdn(int entityId, const QStringList &valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceOsci(int entityId, const QStringList &valueChannelList, int interpolation) override;
    Zera::DspInterfacePtr createDspInterfacePeriodAverage(int entityId, const QStringList &valueChannelList, int maxPeriodCount, int initialPeriodCount) override;
    Zera::DspInterfacePtr createDspInterfaceMode(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceRefProg(int entityId, const QStringList &valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceStatus(int entityId) override;
};

#endif // FACTORYSERVICEINTERFACES_H
