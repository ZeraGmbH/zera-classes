#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacerange.h"
#include "demodspinterfacedft.h"
#include "demodspinterfacefft.h"
#include "demodspinterfacerms.h"
#include "demodspinterfacepower1.h"
#include "demodspinterfacepower2.h"
#include "demodspinterfacesample.h"
#include "demodspinterfacethdn.h"
#include "demodspinterfaceosci.h"

static double generatorFixed() {
    return 1.0;
}

static double generatorReproducibleChange() {
    constexpr double initValue = 0.9;
    static double value = initValue;
    value += 0.001;
    if (value > 1.0)
        value = initValue;
    return value;
}

static double generatorRandom() {
    return (double)rand() / double(RAND_MAX);
}

DemoFactoryServiceInterfaces::DemoFactoryServiceInterfaces(ValueTypes valueType) :
    m_valueType(valueType)
{
    switch(valueType) {
    case RandomValues:
        m_valueGenerator = generatorRandom;
        break;
    case FixedValues:
        m_valueGenerator = generatorFixed;
        break;
    case ReproducableChangeValues:
        m_valueGenerator = generatorReproducibleChange;
        break;
    }
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId,
                                                                                QStringList valueChannelList,
                                                                                bool isReference)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                                 QStringList valueChannelList,
                                                                                 bool isReference)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                               QStringList valueChannelList,
                                                                               bool isReference)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceDft(int entityId,
                                                                          QStringList valueChannelList,
                                                                          int dftOrder)
{
    Q_UNUSED(entityId)
    DemoDspInterfaceDft::ValueTypes dftValueType = m_valueType == FixedValues ?
                                                       DemoDspInterfaceDft::FixedValues :
                                                       DemoDspInterfaceDft::RotatingValues ;
    return std::make_shared<DemoDspInterfaceDft>(valueChannelList, dftOrder, dftValueType);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceFft(int entityId,
                                                                          QStringList valueChannelList,
                                                                          int fftOrder)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceFft>(valueChannelList, fftOrder, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                          QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRms>(valueChannelList, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                             MeasModeSelector* measMode)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfacePower1>(measMode, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfacePower2>(measMode, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceSample(int entityId,
                                                                             QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceSample>(valueChannelList, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                           QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceThdn>(valueChannelList, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                           QStringList valueChannelList,
                                                                           int interpolation)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceOsci>(valueChannelList, interpolation, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId,
                                                                              QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}
