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

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeProg(QStringList valueChannelList, bool isReference)
{
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeObser(QStringList valueChannelList, bool isReference)
{
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRangeAdj(QStringList valueChannelList, bool isReference)
{
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReference);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceDft(QStringList valueChannelList, int dftOrder)
{
    return std::make_shared<DemoDspInterfaceDft>(valueChannelList, dftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceFft(QStringList valueChannelList, int fftOrder)
{
    return std::make_shared<DemoDspInterfaceFft>(valueChannelList, fftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceRms>(valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower1(MeasModeSelector* measMode)
{
    return std::make_shared<DemoDspInterfacePower1>(measMode);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower2(MeasModeSelector *measMode)
{
    return std::make_shared<DemoDspInterfacePower2>(measMode);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceSample(QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceSample>(valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceThdn(QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceThdn>(valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOsci(QStringList valueChannelList, int interpolation)
{
    return std::make_shared<DemoDspInterfaceOsci>(valueChannelList, interpolation);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceMode()
{
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRefProg(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRefAdj()
{
    // no demo specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}
