#ifndef QTUNITTESTSOURCEINTERFACE_H
#define QTUNITTESTSOURCEINTERFACE_H

#include <QObject>
#include "io-interface/iointerfacebase.h"

class IoInterfaceTest : public QObject
{
    Q_OBJECT
public slots:
    void onIoFinish(int ioID, bool error);

private slots:
    void init();

    void generateOOLInterface();
    void generateTypeSet();

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

    void demoDelayFollowsDelay();
    void demoDelayFollowsTimeout();

    void baseCannotClose();
    void demoCanClose();

private:
    void checkIds(tIoInterfaceShPtr interface);
    void checkNotifications(tIoInterfaceShPtr interface, int total, int errors);
    tIoInterfaceShPtr createOpenDemoInterface(int responseDelay = 0);

    int m_ioFinishReceiveCount = 0;
    int m_ioIDReceived = 0;
    int m_errorsReceived = 0;
    QByteArray m_receivedData;
    QList<QByteArray> m_listReceivedData;
};

#endif // QTUNITTESTSOURCEINTERFACE_H
