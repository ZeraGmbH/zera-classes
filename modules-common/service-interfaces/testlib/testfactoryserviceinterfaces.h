#ifndef TESTFACTORYSERVICEINTERFACES_H
#define TESTFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include "testdspinterface.h"

class TestFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOther() override;
    const QList<TestDspInterfacePtr>& getInterfaceList() const;
    void clearInterfaceList();
private:
    DspInterfacePtr createDspInterfaceCommon(int interruptNoHandled, QStringList valueChannelList);
    QList<TestDspInterfacePtr> m_dspInterfaces;
};

typedef std::shared_ptr<TestFactoryServiceInterfaces> TestFactoryServiceInterfacesPtr;

#endif // TESTFACTORYSERVICEINTERFACES_H
