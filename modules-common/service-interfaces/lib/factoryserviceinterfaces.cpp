#include "factoryserviceinterfaces.h"

DspInterfacePtr FactoryServiceInterfaces::getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::getDspInterfaceOther()
{
    return std::make_shared<Zera::cDSPInterface>();
}
