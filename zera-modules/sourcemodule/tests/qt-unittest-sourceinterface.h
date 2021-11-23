#ifndef QTUNITTESTSOURCEINTERFACE_H
#define QTUNITTESTSOURCEINTERFACE_H

#include <QObject>
#include "sourceinterface.h"

class SourceInterfaceTest : public QObject
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

private:
    void checkIds(tSourceInterfaceShPtr interface);
    void checkNotifications(tSourceInterfaceShPtr interface, int total, int errors);

    int m_ioFinishReceiveCount = 0;
    int m_ioIDReceived = 0;
    int m_errorsReceived = 0;
    QByteArray m_receivedData;
    QList<QByteArray> m_listReceivedData;
};

#endif // QTUNITTESTSOURCEINTERFACE_H
