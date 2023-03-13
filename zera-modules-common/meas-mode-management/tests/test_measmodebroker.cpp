#include "test_measmodebroker.h"
#include "measmodebroker.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyphasesvar.h"
#include <QTest>

class MeasChannelSingleton
{
public:
    static const MeasSystemChannels get() {
        return {{0,1}, {2,3}, {4,5}};
    }
};

class DspCmdGeneratorForTest
{
public:
    static QStringList getCmdsA(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen)
    {
        Q_UNUSED(idGen)
        Q_UNUSED(measChannelPairList)
        QStringList ret;
        ret.append(QString("%1").arg(dspSelectCode));
        ret.append("getCmdsA");
        return ret;
    }
    static QStringList getCmdsB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen)
    {
        Q_UNUSED(idGen)
        Q_UNUSED(measChannelPairList)
        QStringList ret;
        ret.append(QString("%1").arg(dspSelectCode));
        ret.append("getCmdsB");
        return ret;
    }
};

class TestDspModeFunctionCatalog
{
public:
    static const ModeNameFunctionHash &get()
    {
        if(m_hash.isEmpty()) {
            m_hash["A"] = { mXlw, DspCmdGeneratorForTest::getCmdsA,
                    []{ return std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("000"), 1);} };
            m_hash["A1"] = { mXlw, DspCmdGeneratorForTest::getCmdsA,
                    []{ return std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("000"), 1);} };
            m_hash["B"] = { m3lw, DspCmdGeneratorForTest::getCmdsB,
                    []{ return std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("000"), 3);} };
        }
        return m_hash;
    }
private:
    static ModeNameFunctionHash m_hash;
};

ModeNameFunctionHash TestDspModeFunctionCatalog::m_hash;

QTEST_MAIN(test_measmodebroker)

void test_measmodebroker::getOneCheckDspCmd()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected = DspCmdGeneratorForTest::getCmdsA(ret.dspSelectCode, MeasChannelSingleton::get(), dummyIdGen);
    QCOMPARE(expected, ret.dspCmdList);
}

void test_measmodebroker::getOneCheckSelectionInDspCmd()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected = DspCmdGeneratorForTest::getCmdsA(ret.dspSelectCode+1, MeasChannelSingleton::get(), dummyIdGen);
    QVERIFY(expected != ret.dspCmdList);
}

void test_measmodebroker::getTwoCheckDspCmd()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected1 = DspCmdGeneratorForTest::getCmdsA(ret1.dspSelectCode, MeasChannelSingleton::get(), dummyIdGen);
    QCOMPARE(expected1, ret1.dspCmdList);

    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("B", MeasChannelSingleton::get());
    QStringList expected2 = DspCmdGeneratorForTest::getCmdsB(ret2.dspSelectCode, MeasChannelSingleton::get(), dummyIdGen);
    QCOMPARE(expected2, ret2.dspCmdList);
}

void test_measmodebroker::getTwoModesWithSameDspCmdSecondCmdEmpty()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret;
    ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected1 = DspCmdGeneratorForTest::getCmdsA(ret.dspSelectCode, MeasChannelSingleton::get(), dummyIdGen);
    QCOMPARE(expected1, ret.dspCmdList);

    ret = broker.getMeasMode("A1", MeasChannelSingleton::get());
    QStringList expected2;
    QCOMPARE(expected2, ret.dspCmdList);
}

void test_measmodebroker::getTwoModesWithSameCheckAllReturnsSet()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    QVERIFY(ret1.isValid());
    QVERIFY(!ret1.dspCmdList.isEmpty());
    QVERIFY(ret1.dspSelectCode != 0);
    QVERIFY(ret1.phaseStrategy != nullptr);
    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("A1", MeasChannelSingleton::get());
    QVERIFY(ret2.isValid());
    QVERIFY(ret2.dspCmdList.isEmpty());
    QVERIFY(ret2.dspSelectCode != 0);
    QVERIFY(ret2.phaseStrategy != nullptr);
}

void test_measmodebroker::getTwoCheckDspSelectionSame()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("A1", MeasChannelSingleton::get());
    QVERIFY(ret1.isValid());
    QCOMPARE(ret1.dspSelectCode, ret2.dspSelectCode);
}

void test_measmodebroker::singleCheckDspCmdIsValid()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QVERIFY(ret.isValid());
}

void test_measmodebroker::getFooIsInvalid()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("FOO", MeasChannelSingleton::get());
    QVERIFY(!ret.isValid());
}

void test_measmodebroker::getTwoWithDiffDspSelection()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("B", MeasChannelSingleton::get());
    QVERIFY(ret1.isValid());
    QVERIFY(ret2.isValid());
    QVERIFY(ret1.dspSelectCode != ret2.dspSelectCode);
}

void test_measmodebroker::getTwoCheckPhaseStartegies()
{
    DspChainIdGen dummyIdGen;
    MeasModeBroker broker(TestDspModeFunctionCatalog::get(), dummyIdGen);
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("B", MeasChannelSingleton::get());

    QVERIFY(ret1.isValid());
    QVERIFY(ret1.phaseStrategy->isVarMask());
    QCOMPARE(ret1.phaseStrategy->getActiveMeasSysCount(), 1);
    QVERIFY(ret2.isValid());
    QVERIFY(!ret2.phaseStrategy->isVarMask());
    QCOMPARE(ret2.phaseStrategy->getActiveMeasSysCount(), 3);
}

