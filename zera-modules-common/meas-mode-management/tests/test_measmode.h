#ifndef TEST_MEASMODE_H
#define TEST_MEASMODE_H

#include <QObject>

class test_measmode : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void gettersReportProperCtorParams();
    void validPhaseChangeSignal();
    void fixedStrategyCannotChangePhase();
    void invalidModeName();
    void invalidForMeasSystemTooLarge();
    void invalidForMeasSystemTooSmall();
    void invalidCannotChangePhases();
    void tooLongMaskNotAccepted();
    void onePhaseMeasSystem();
    void twoPhaseMeasSystem();
};

#endif // TEST_MEASMODE_H
