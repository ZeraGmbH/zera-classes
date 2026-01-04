#ifndef QTUNITTESTSOURCEIODEVICE_H
#define QTUNITTESTSOURCEIODEVICE_H

#include "iodevicebase.h"

class test_iodevice : public QObject
{
    Q_OBJECT
public slots:
    void onIoFinish(int ioID, bool error);

private slots:
    void initTestCase();
    void init();

    void generateBrokenIoDeviceForOutOfLimitType();

    void baseReturnsIds();
    void demoReturnsIds();
    void serialReturnsIds();

    void baseCannotOpen();
    void demoCanOpen();
    void serialCannotOpen();

    void baseReportsErrorClose();
    void baseReportsErrorOpen();
    void demoReportsErrorClose();
    void demoReportsNoErrorOpen();
    void serialReportsErrorClose();

    void demoDelayNotOpen();
    void demoDelayOpenDontWait();
    void demoOpenDelayWait();

    void demoResponseList();
    void demoResponseListDelay();
    void demoResponseListErrorInjection();
    void demoResponseAlwaysError();

    void demoDelayFollowsDelay();
    void demoDelayFollowsTimeout();

    void baseCannotClose();
    void demoCanClose();

private:
    void checkIds(IoDeviceBase::Ptr ioDevice);
    void checkNotifications(IoDeviceBase::Ptr ioDevice, int total, int errors);
    IoDeviceBase::Ptr createOpenDemoIoDeviceWithDelayAndConnected(int responseDelay = 0);

    int m_ioFinishReceiveCount = 0;
    int m_ioIDReceived = 0;
    int m_errorsReceived = 0;
    IoTransferDataSingle::Ptr m_ioDataForSingleUse;
    QList<QByteArray> m_listReceivedData;
};

#endif // QTUNITTESTSOURCEIODEVICE_H
