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

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceDft>(valueChannelList, dftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceFft>(valueChannelList, fftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceRms>(valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower1(int entityId, MeasModeSelector* measMode)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfacePower1>(measMode);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower2(int entityId, MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfacePower2>(measMode);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceSample(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceSample>(valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceThdn(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceThdn>(valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation)
{
    Q_UNUSED(entityId)
    return std::make_shared<DemoDspInterfaceOsci>(valueChannelList, interpolation);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    Q_UNUSED(entityId)
    return std::make_shared<Zera::cDSPInterface>();
}
