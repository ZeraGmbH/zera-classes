#ifndef ABSTRACTFACTORYSERVICEINTERFACES_H
#define ABSTRACTFACTORYSERVICEINTERFACES_H

#include <dspinterface.h>
#include <memory>

typedef std::shared_ptr<Zera::cDSPInterface> DspInterfacePtr;

class AbstractFactoryServiceInterfaces
{
public:
    virtual DspInterfacePtr getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr getDspInterfaceOther() = 0;
};

typedef std::unique_ptr<AbstractFactoryServiceInterfaces> AbstractFactoryServiceInterfacesPtr;

#endif // ABSTRACTFACTORYSERVICEINTERFACES_H
