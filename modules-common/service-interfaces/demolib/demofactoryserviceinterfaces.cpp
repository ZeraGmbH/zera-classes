#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacedft.h"
#include "demodspinterfacefft.h"
#include "demodspinterfacerms.h"

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceDft>(interruptNoHandled, valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder)
{
    return std::make_shared<DemoDspInterfaceFft>(interruptNoHandled, valueChannelList, fftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceRms>(interruptNoHandled, valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOther()
{
    // no demo specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}
