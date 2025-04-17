#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include <functional>

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    enum ValueTypes {
        RandomValues,
        FixedValues,
        ReproducableChangeValues
    };
    explicit DemoFactoryServiceInterfaces(ValueTypes valueType = RandomValues);

    Zera::DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector *measMode) override;
    Zera::DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfaceSample(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) override;
    Zera::DspInterfacePtr createDspInterfaceMode(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;
private:
    ValueTypes m_valueType = RandomValues;
    std::function<double()> m_valueGenerator;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
