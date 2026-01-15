#ifndef TEST_SAMPLE_SET_CHANNEL_H
#define TEST_SAMPLE_SET_CHANNEL_H

#include <QObject>

class test_sample_set_channel : public QObject
{
    Q_OBJECT
private slots:
    void startUL1NoFixNoAutoSetInvalid();
    void startUL1NoFixNoAutoSetUL2();
    void startIL1NoFixAutoSetUL2NoChange();
    void startFixAutoNotSetUL2NoChange();
};

#endif // TEST_SAMPLE_SET_CHANNEL_H
