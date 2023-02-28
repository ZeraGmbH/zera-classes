#ifndef TEST_MEASMODE_H
#define TEST_MEASMODE_H

#include <QObject>

class test_measmode : public QObject
{
    Q_OBJECT
private slots:
    void gettersReportProperCtorParams();
    void validPhaseChangeSignal();
    void invalidPhaseChangeSignal();
};

#endif // TEST_MEASMODE_H
