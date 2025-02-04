#ifndef TEST_RANGE_AUTOMATIC_H
#define TEST_RANGE_AUTOMATIC_H

#include "testdspinterface.h"
#include <QObject>

class test_range_automatic : public QObject
{
    Q_OBJECT
private slots:
    void defaultRangesAndSetting();
    void testRangeAutomatic();
    void enableAndDisableRangeAutomatic();
    void softOverloadWithRangeAutomatic();
private:
    void fireNewRmsValues(const TestDspInterfacePtr &dspInterface, float rmsValue);
};

#endif // TEST_RANGE_AUTOMATIC_H
