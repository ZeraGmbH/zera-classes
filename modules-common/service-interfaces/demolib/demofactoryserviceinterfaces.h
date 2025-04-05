#ifndef DEMOFACTORYSERVICEINTERFACES_H
#define DEMOFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include <functional>

class DemoFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    enum ValueTypes {
        RandomValues,
        FixedValues
    };
    DemoFactoryServiceInterfaces(ValueTypes valueType = RandomValues);

    DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector *measMode) override;
    DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    DspInterfacePtr createDspInterfaceSample(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) override;
    DspInterfacePtr createDspInterfaceMode(int entityId) override;
    DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;
private:
    ValueTypes m_valueType;
    std::function<double()> m_valueGenerator;
};

#endif // DEMOFACTORYSERVICEINTERFACES_H
