#include "test_measmodebroker.h"
#include "measmodebroker.h"
#include <QTest>

class MeasChannelSingleton
{
public:
    static const QList<MeasSystemChannels> get() {
        return {{0,1}, {2,3}, {4,5}};
    }
};

class DspCmdGeneratorForTest
{
public:
    static QStringList getCmdsA(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList)
    {
        Q_UNUSED(measChannelPairList)
        QStringList ret;
        ret.append(QString("%1").arg(dspSelectCode));
        ret.append("getCmdsA");
        return ret;
    }
    static QStringList getCmdsB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList)
    {
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
            m_hash["A"] = { mXlw, DspCmdGeneratorForTest::getCmdsA};
            m_hash["A1"] = { mXlw, DspCmdGeneratorForTest::getCmdsA};
            m_hash["B"] = { m3lw, DspCmdGeneratorForTest::getCmdsB};
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
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected = DspCmdGeneratorForTest::getCmdsA(ret.dspSelectCode, MeasChannelSingleton::get());
    QCOMPARE(expected, ret.dspCmdList);
}

void test_measmodebroker::getOneCheckSelectionInDspCmd()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected = DspCmdGeneratorForTest::getCmdsA(ret.dspSelectCode+1, MeasChannelSingleton::get());
    QVERIFY(expected != ret.dspCmdList);
}

void test_measmodebroker::getTwoCheckDspCmd()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected1 = DspCmdGeneratorForTest::getCmdsA(ret1.dspSelectCode, MeasChannelSingleton::get());
    QCOMPARE(expected1, ret1.dspCmdList);

    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("B", MeasChannelSingleton::get());
    QStringList expected2 = DspCmdGeneratorForTest::getCmdsB(ret2.dspSelectCode, MeasChannelSingleton::get());
    QCOMPARE(expected2, ret2.dspCmdList);
}

void test_measmodebroker::getTwoModesWithSameDspCmdSecondCmdEmpty()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret;
    ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QStringList expected1 = DspCmdGeneratorForTest::getCmdsA(ret.dspSelectCode, MeasChannelSingleton::get());
    QCOMPARE(expected1, ret.dspCmdList);

    ret = broker.getMeasMode("A1", MeasChannelSingleton::get());
    QStringList expected2;
    QCOMPARE(expected2, ret.dspCmdList);
}

void test_measmodebroker::getTwoCheckDspSelectionSame()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("A1", MeasChannelSingleton::get());
    QVERIFY(ret1.isValid());
    QCOMPARE(ret1.dspSelectCode, ret2.dspSelectCode);
}

void test_measmodebroker::singleCheckDspCmdIsValid()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("A", MeasChannelSingleton::get());
    QVERIFY(ret.isValid());
}

void test_measmodebroker::getFooIsInvalid()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret = broker.getMeasMode("FOO", MeasChannelSingleton::get());
    QVERIFY(!ret.isValid());
}

void test_measmodebroker::getTwoWithDiffDspSelection()
{
    MeasModeBroker broker(TestDspModeFunctionCatalog::get());
    MeasModeBroker::BrokerReturn ret1 = broker.getMeasMode("A", MeasChannelSingleton::get());
    MeasModeBroker::BrokerReturn ret2 = broker.getMeasMode("B", MeasChannelSingleton::get());
    QVERIFY(ret1.isValid());
    QVERIFY(ret2.isValid());
    QVERIFY(ret1.dspSelectCode != ret2.dspSelectCode);
}

