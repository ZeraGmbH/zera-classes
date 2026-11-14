#ifndef TEST_RANGE_TO_ZERO_CLAMP_CHEATING_H
#define TEST_RANGE_TO_ZERO_CLAMP_CHEATING_H

#include <QObject>

class test_range_to_zero_clamp_cheating : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void injectActualValuesWithCheating();
    void injectActualValuesWithCheatingAndRangeChanged();
    void injectIncreasingActualValuesWithCheatingEnabled();
    void injectActualValuesCheatingEnabledWithPreScaling();
};

#endif // TEST_RANGE_TO_ZERO_CLAMP_CHEATING_H
