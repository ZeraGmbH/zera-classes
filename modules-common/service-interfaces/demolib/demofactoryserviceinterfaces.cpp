#include "demofactoryserviceinterfaces.h"
#include "demodspinterfacerms.h"

DspInterfacePtr DemoFactoryServiceInterfaces::getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    return std::make_shared<DemoDspInterfaceRms>(interruptNoHandled, valueChannelList);
}

DspInterfacePtr DemoFactoryServiceInterfaces::getDspInterfaceOther()
{
    // no demo specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}
