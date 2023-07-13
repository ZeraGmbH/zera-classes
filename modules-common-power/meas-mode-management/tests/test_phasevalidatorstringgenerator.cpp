#include "test_phasevalidatorstringgenerator.h"
#include "veinvalidatorphasestringgenerator.h"
#include <QTest>

QTEST_MAIN(test_phasevalidatorstringgenerator)

void test_phasevalidatorstringgenerator::zeroPhases()
{
    QCOMPARE(VeinValidatorPhaseStringGenerator::generate(0).size(), 0);
}

void test_phasevalidatorstringgenerator::onePhase()
{
    QStringList phaseValidList = VeinValidatorPhaseStringGenerator::generate(1);
    QCOMPARE(phaseValidList.size(), 2);
    QVERIFY(phaseValidList.contains("0"));
    QVERIFY(phaseValidList.contains("1"));
}

void test_phasevalidatorstringgenerator::twoPhases()
{
    QStringList phaseValidList = VeinValidatorPhaseStringGenerator::generate(2);
    QCOMPARE(phaseValidList.size(), 4);
    QVERIFY(phaseValidList.contains("00"));
    QVERIFY(phaseValidList.contains("01"));
    QVERIFY(phaseValidList.contains("10"));
    QVERIFY(phaseValidList.contains("11"));
}

void test_phasevalidatorstringgenerator::numPhasesEqualMax()
{
    QStringList phaseValidList = VeinValidatorPhaseStringGenerator::generate(3,3);
    QVERIFY(phaseValidList.size()>0);
}

void test_phasevalidatorstringgenerator::morePhasesThanMax()
{
    QStringList phaseValidList = VeinValidatorPhaseStringGenerator::generate(4,3);
    QCOMPARE(phaseValidList.size(), 0);
}
