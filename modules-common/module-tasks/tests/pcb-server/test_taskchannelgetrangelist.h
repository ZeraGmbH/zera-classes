#ifndef TEST_TASKCHANNELGETRANGELIST_H
#define TEST_TASKCHANNELGETRANGELIST_H

#include <QObject>

class test_taskchannelgetrangelist : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsRangeListProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETRANGELIST_H
