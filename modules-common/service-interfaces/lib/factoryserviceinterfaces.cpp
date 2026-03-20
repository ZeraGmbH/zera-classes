#include "factoryserviceinterfaces.h"

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId,
                                                                            const QStringList &valueChannelList,
                                                                            bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                             const QStringList &valueChannelList,
                                                                             bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                           const QStringList &valueChannelList,
                                                                           bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDft(int entityId,
                                                                      const QStringList &valueChannelList,
                                                                      int dftOrder)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(dftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceFft(int entityId,
                                                                      const QStringList &valueChannelList,
                                                                      int fftOrder)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(fftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                      const QStringList &valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                         MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    Q_UNUSED(measMode)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                         MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    Q_UNUSED(measMode)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                       const QStringList &valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                       const QStringList &valueChannelList,
                                                                       int interpolation)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(interpolation)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePeriodAverage(int entityId,
                                                                                const QStringList &valueChannelList,
                                                                                int maxPeriodCount, int initialPeriodCount)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(maxPeriodCount)
    Q_UNUSED(initialPeriodCount)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRefProg(int entityId,
                                                                          const QStringList &valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceStatus(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}

Zera::DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDspSuper(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}
