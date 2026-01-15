#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacerange.h"
#include "demodspinterfacedft.h"
#include "demodspinterfacefft.h"
#include "demodspinterfacerms.h"
#include "demodspinterfacepower1.h"
#include "demodspinterfacepower2.h"
#include "demodspinterfacethdn.h"
#include "demodspinterfaceosci.h"

static double generatorFixed(int entityId) {
    Q_UNUSED(entityId)
    return 1.0;
}

static double generatorReproducibleChange(int entityId) {
    constexpr double initValue = 0.9;
    static QMap<int, double> values;
    double value = initValue;
    if (values.contains(entityId))
        value = values[entityId];
    value += 0.001;
    if (value > 1.0)
        value = initValue;
    values[entityId] = value;
    return value;
}

static double generatorRandom(int entityId) {
    Q_UNUSED(entityId)
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
    return std::make_shared<DemoDspInterfaceRange>(entityId, valueChannelList, isReference, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                                 QStringList valueChannelList,
                                                                                 bool isReference)
{
    return std::make_shared<DemoDspInterfaceRange>(entityId, valueChannelList, isReference, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                               QStringList valueChannelList,
                                                                               bool isReference)
{
    return std::make_shared<DemoDspInterfaceRange>(entityId, valueChannelList, isReference, m_valueGenerator);
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
    return std::make_shared<DemoDspInterfaceFft>(entityId, valueChannelList, fftOrder, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                          QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceRms>(entityId, valueChannelList, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                             MeasModeSelector* measMode)
{
    return std::make_shared<DemoDspInterfacePower1>(entityId, measMode, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                             MeasModeSelector *measMode)
{
    return std::make_shared<DemoDspInterfacePower2>(entityId, measMode, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                           QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceThdn>(entityId, valueChannelList, m_valueGenerator);
}

Zera::DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                           QStringList valueChannelList,
                                                                           int interpolation)
{
    return std::make_shared<DemoDspInterfaceOsci>(entityId, valueChannelList, interpolation, m_valueGenerator);
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
