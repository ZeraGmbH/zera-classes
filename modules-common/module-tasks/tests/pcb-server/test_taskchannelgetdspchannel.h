#ifndef TEST_TASKCHANNELGETDSPCHANNEL_H
#define TEST_TASKCHANNELGETDSPCHANNEL_H

#include <QObject>

class test_taskchannelgetdspchannel : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsDspChannelProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKCHANNELGETDSPCHANNEL_H
