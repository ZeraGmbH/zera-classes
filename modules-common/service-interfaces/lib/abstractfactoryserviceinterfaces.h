#ifndef ABSTRACTFACTORYSERVICEINTERFACES_H
#define ABSTRACTFACTORYSERVICEINTERFACES_H

#include <dspinterface.h>
#include <memory>

typedef std::shared_ptr<Zera::cDSPInterface> DspInterfacePtr;

class AbstractFactoryServiceInterfaces
{
public:
    virtual DspInterfacePtr createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceOther() = 0;
};

typedef std::unique_ptr<AbstractFactoryServiceInterfaces> AbstractFactoryServiceInterfacesPtr;

#endif // ABSTRACTFACTORYSERVICEINTERFACES_H
