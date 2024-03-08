#include "factoryserviceinterfaces.h"

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRange(QStringList valueChannelList, bool isReferencce)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(isReferencce)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceDft(QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(dftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceFft(QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(valueChannelList)
    Q_UNUSED(fftOrder)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceRms(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<Zera::cDSPInterface>();
}

DspInterfacePtr FactoryServiceInterfaces::createDspInterfaceOther()
{
    return std::make_shared<Zera::cDSPInterface>();
}
