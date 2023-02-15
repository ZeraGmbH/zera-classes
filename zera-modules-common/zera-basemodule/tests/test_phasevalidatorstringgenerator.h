#ifndef TEST_PHASEVALIDATORSTRINGGENERATOR_H
#define TEST_PHASEVALIDATORSTRINGGENERATOR_H

#include <QObject>

class test_phasevalidatorstringgenerator : public QObject
{
    Q_OBJECT
private slots:
    void zeroPhases();
    void onePhase();
    void twoPhases();
    void morePhasesThanMax();
};

#endif // TEST_PHASEVALIDATORSTRINGGENERATOR_H
