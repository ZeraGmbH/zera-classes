#include "test_dspatomiccommandgen.h"
#include "dspatomiccommandgen.h"
#include <QTest>

QTEST_MAIN(test_dspatomiccommandgen)

void test_dspatomiccommandgen::startChain()
{
    auto cmdActive = DspAtomicCommandGen::getCmdStartChainActive(0x0101);
    QCOMPARE(cmdActive, "STARTCHAIN(1,1,0x0101)");
    auto cmdInactive = DspAtomicCommandGen::getStartChainInactive(0x0101);
    QCOMPARE(cmdInactive, "STARTCHAIN(0,1,0x0101)");
}

void test_dspatomiccommandgen::stopChain()
{
    auto cmd = DspAtomicCommandGen::getStopChain(0x0102);
    QCOMPARE(cmd, "STOPCHAIN(1,0x0102)");
}

void test_dspatomiccommandgen::activateChain()
{
    auto cmd = DspAtomicCommandGen::getActivateChain(0x0102);
    QCOMPARE(cmd, "ACTIVATECHAIN(1,0x0102)");
}

void test_dspatomiccommandgen::deactivateChain()
{
    auto cmd = DspAtomicCommandGen::getDeactivateChain(0x0102);
    QCOMPARE(cmd, "DEACTIVATECHAIN(1,0x0102)");
}


