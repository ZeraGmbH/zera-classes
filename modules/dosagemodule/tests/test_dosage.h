#ifndef TEST_DOSAGE_H
#define TEST_DOSAGE_H


#include <QObject>

class test_dosage : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testDosageAllBelowLimits();
    void testDosageRmsUpperLimits();
    void testDosagePowUpperLimits();
};

#endif // TEST_DOSAGE_H
