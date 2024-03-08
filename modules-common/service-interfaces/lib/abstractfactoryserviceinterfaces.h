#ifndef ABSTRACTFACTORYSERVICEINTERFACES_H
#define ABSTRACTFACTORYSERVICEINTERFACES_H

#include <dspinterface.h>
#include <memory>

typedef std::shared_ptr<Zera::cDSPInterface> DspInterfacePtr;

class AbstractFactoryServiceInterfaces
{
public:
    virtual DspInterfacePtr createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce) = 0;
    virtual DspInterfacePtr createDspInterfaceDft(QStringList valueChannelList, int dftOrder) = 0;
    virtual DspInterfacePtr createDspInterfaceFft(QStringList valueChannelList, int fftOrder) = 0;
    virtual DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) = 0;
    virtual DspInterfacePtr createDspInterfaceOther() = 0;
};

typedef std::shared_ptr<AbstractFactoryServiceInterfaces> AbstractFactoryServiceInterfacesPtr;

#endif // ABSTRACTFACTORYSERVICEINTERFACES_H
