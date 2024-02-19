#include "factoryserviceinterfaces.h"

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceOther()
{
    return std::make_shared<Zera::cDSPInterface>();
}
