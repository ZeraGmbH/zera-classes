#include "factoryserviceinterfaces.h"

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReferencce)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(dftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    Q_UNUSED(fftOrder)
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
