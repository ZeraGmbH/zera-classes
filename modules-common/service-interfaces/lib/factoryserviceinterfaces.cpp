#include "factoryserviceinterfaces.h"

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeProg(QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeObser(QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRangeAdj(QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReference)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDft(QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(dftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceFft(QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(fftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRms(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePower1(MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfacePower2(MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceSample(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceThdn(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceOsci(QStringList valueChannelList, int interpolation)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(interpolation)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceMode()
{
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRefProg(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRefAdj()
{
    return std::make_shared<Zera::cDSPInterface>();
}
