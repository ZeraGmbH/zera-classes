#include "factoryserviceinterfaces.h"

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(dftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(fftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePower1(int entityId, MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    Q_UNUSED(measMode)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePower2(int entityId, MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    Q_UNUSED(measMode)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceSample(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceThdn(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(interpolation)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRefProg(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}
