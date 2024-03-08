#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacerange.h"
#include "demodspinterfacedft.h"
#include "demodspinterfacefft.h"
#include "demodspinterfacerms.h"
#include "demodspinterfacepower1.h"

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce)
{
    return std::make_shared<DemoDspInterfaceRange>(interruptNoHandled, valueChannelList, isReferencce);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList, int dftOrder)
{
    return std::make_shared<DemoDspInterfaceDft>(interruptNoHandled, valueChannelList, dftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder)
{
    return std::make_shared<DemoDspInterfaceFft>(interruptNoHandled, valueChannelList, fftOrder);
}

DspInterfacePtr DemoFactoryServiceInterfaces::createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceRms>(interruptNoHandled, valueChannelList);
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
