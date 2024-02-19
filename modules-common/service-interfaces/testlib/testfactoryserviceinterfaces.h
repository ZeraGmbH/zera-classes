#ifndef TESTFACTORYSERVICEINTERFACES_H
#define TESTFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include "testdspinterface.h"

class TestFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) override;
    DspInterfacePtr getDspInterfaceOther() override;
    const QList<TestDspInterfacePtr>& getInterfaceList() const;
    void clearInterfaceList();
private:
    QList<TestDspInterfacePtr> m_dspInterfaces;
};

typedef std::shared_ptr<TestFactoryServiceInterfaces> TestFactoryServiceInterfacesPtr;

#endif // TESTFACTORYSERVICEINTERFACES_H
