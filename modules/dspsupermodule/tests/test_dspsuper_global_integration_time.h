#ifndef TEST_DSPSUPER_GLOBAL_INTEGRATION_TIME_H
#define TEST_DSPSUPER_GLOBAL_INTEGRATION_TIME_H

#include <QObject>

class test_dspsuper_global_integration_time : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void integrationComponentsFindNoneOnMinimal();
    void integrationComponentsFindTwoTime();
    void integrationComponentsFindTwoPeriod();
    void integrationComponentsFindTwoTimeTwoPeriod();
};

#endif // TEST_DSPSUPER_GLOBAL_INTEGRATION_TIME_H
