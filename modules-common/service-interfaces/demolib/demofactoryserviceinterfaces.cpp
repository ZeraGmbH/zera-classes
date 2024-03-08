#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacerange.h"
#include "demodspinterfacedft.h"
#include "demodspinterfacefft.h"
#include "demodspinterfacerms.h"

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

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceRms>(interruptNoHandled, valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceOther()
{
    // no demo specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}
