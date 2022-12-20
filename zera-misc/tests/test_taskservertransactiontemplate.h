#ifndef TEST_TASKSERVERTRANSACTIONTEMPLATE_H
#define TEST_TASKSERVERTRANSACTIONTEMPLATE_H

#include <QObject>

class test_taskservertransactiontemplate : public QObject
{
    Q_OBJECT
private slots:
    void actionOnAck();
    void noActionOnNack();
    void okSignalOnAck();
    void errSignalOnNack();
    void noReceiveOnOther();
    void errReceiveOnTcpError();
};

#endif // TEST_TASKSERVERTRANSACTIONTEMPLATE_H
