#ifndef TEST_TASKCHANNELGETUNIT_H
#define TEST_TASKCHANNELGETUNIT_H

#include <QObject>

class test_taskchannelgetunit : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsAliasProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETUNIT_H
