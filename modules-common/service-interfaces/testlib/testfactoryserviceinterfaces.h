#ifndef TESTFACTORYSERVICEINTERFACES_H
#define TESTFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include "testdspinterface.h"

class TestFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) override;
    DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) override;
    DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) override;
    DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) override;
    DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    DspInterfacePtr createDspInterfaceSample(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) override;
    DspInterfacePtr createDspInterfaceMode(int entityId) override;
    DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) override;
    DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;
    const QList<TestDspInterfacePtr>& getInterfaceList() const;
    void clearInterfaceList();
private:
    DspInterfacePtr createDspInterfaceCommon(int interruptNoHandled, QStringList valueChannelList);
    QList<TestDspInterfacePtr> m_dspInterfaces;
};

typedef std::shared_ptr<TestFactoryServiceInterfaces> TestFactoryServiceInterfacesPtr;

#endif // TESTFACTORYSERVICEINTERFACES_H
