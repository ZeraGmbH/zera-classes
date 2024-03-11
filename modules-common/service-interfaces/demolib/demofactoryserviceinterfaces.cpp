#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacerange.h"
#include "demodspinterfacedft.h"
#include "demodspinterfacefft.h"
#include "demodspinterfacerms.h"
#include "demodspinterfacepower1.h"

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRange(QStringList valueChannelList, bool isReferencce)
{
    return std::make_shared<DemoDspInterfaceRange>(valueChannelList, isReferencce);
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

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfacePower1(int interruptNoHandled, MeasModeSelector* measMode)
{
    return std::make_shared<DemoDspInterfacePower1>(interruptNoHandled, measMode);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOther()
{
    // no demo specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}
