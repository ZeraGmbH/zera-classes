#ifndef TESTFACTORYSERVICEINTERFACES_H
#define TESTFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include "testdspinterface.h"
#include <QMap>

class TestFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    enum DSPInterfaceType
    {
        MODULEPROG,
        OBSERVER,
        ADJUST
    };
    void resetInterfaces() override;
    Zera::DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfaceSample(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) override;
    Zera::DspInterfacePtr createDspInterfaceMode(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;

    const QList<TestDspInterfacePtr>& getInterfaceList() const;
    TestDspInterfacePtr getInterface(int entityId, DSPInterfaceType dspInterfaceType);

private:
    Zera::DspInterfacePtr createDspInterfaceCommon(int entityId,
                                             DSPInterfaceType interfaceType,
                                             int interruptNoHandled,
                                             QStringList valueChannelList);
    QList<TestDspInterfacePtr> m_dspInterfaces;
    QMap<int, QMap<DSPInterfaceType, TestDspInterfacePtr>> m_dspInterfacesTyped;
};

typedef std::shared_ptr<TestFactoryServiceInterfaces> TestFactoryServiceInterfacesPtr;

#endif // TESTFACTORYSERVICEINTERFACES_H
