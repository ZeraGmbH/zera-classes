#ifndef TEST_DOSAGE_H
#define TEST_DOSAGE_H


#include <QObject>

class test_dosage : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testDosageMaxAllBelowLimits();
    void testDosageMaxRmsUpperLimits();
    void testDosageMaxPowUpperLimits();
    void testDosageMinPowBelowLimits();
    void testDosageMinPowUpperLimits();
};

#endif // TEST_DOSAGE_H
