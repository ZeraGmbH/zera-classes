#ifndef TEST_RANGE_AUTOMATIC_H
#define TEST_RANGE_AUTOMATIC_H

#include <QObject>

class test_range_automatic : public QObject
{
    Q_OBJECT
private slots:
    void defaultRangesAndSetting();
    void testRangeAutomatic();
    void enableAndDisableRangeAutomatic();
    void softOverloadWithRangeAutomatic();
};

#endif // TEST_RANGE_AUTOMATIC_H
