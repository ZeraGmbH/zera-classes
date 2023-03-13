#include "test_dspchainidgen.h"
#include "dspchainidgen.h"
#include <QTest>

QTEST_MAIN(test_dspchainidgen)

void test_dspchainidgen::main1()
{
    DspChainIdGen gen1(1);
    QCOMPARE(gen1.getNextChainId(), "0x0101");
    QCOMPARE(gen1.getNextChainId(), "0x0102");
    QCOMPARE(gen1.getNextChainId(), "0x0103");
    for(int i=0; i<0x10; i++)
        gen1.getNextChainId();
    QCOMPARE(gen1.getNextChainId(), "0x0114");
}

void test_dspchainidgen::main2()
{
    DspChainIdGen gen1(2);
    QCOMPARE(gen1.getNextChainId(), "0x0201");
    QCOMPARE(gen1.getNextChainId(), "0x0202");
    QCOMPARE(gen1.getNextChainId(), "0x0203");
    for(int i=0; i<0x10; i++)
        gen1.getNextChainId();
    QCOMPARE(gen1.getNextChainId(), "0x0214");
}
