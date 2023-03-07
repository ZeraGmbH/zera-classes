#ifndef TEST_MEASMODE_H
#define TEST_MEASMODE_H

#include <QObject>

class test_measmode : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void fooIsInvalid();
    void defaultCtorIsInvalid();
    void gettersReportProperCtorParams();
    void validCanChangeMask();
    void fixedStrategyCannotChangePhase();
    void invalidForMeasSystemTooLarge();
    void invalidForMeasSystemTooSmall();
    void invalidCannotChangePhases();
    void tooLongMaskNotAccepted();
    void maskWithInvalidCharsNotAccepted();
    void onePhaseMeasSystem();
    void twoPhaseMeasSystem();
    void phaseGetter();
};

#endif // TEST_MEASMODE_H
