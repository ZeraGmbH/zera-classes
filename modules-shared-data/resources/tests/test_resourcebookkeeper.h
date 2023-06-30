#ifndef TEST_RESOURCEBOOKKEEPER_H
#define TEST_RESOURCEBOOKKEEPER_H

#include <QObject>

class test_resourcebookkeeper : public QObject
{
    Q_OBJECT
private slots:
    void initialAmountTotal();
    void initialAmountAllocated();
    void initialAmountFree();
    void singleAlloc();
    void allocTooMuchDenied();
    void negAllocDenied();
    void twoAllocsOneClient();
    void twoAllocsTwoClients();
    void allocAndFree();
    void onInitialFreeFiresNoSignal();
    void allocFreeTwice();
    void killingKeeperFreesAllClients();
    void noAllocsOnDeadKeeper();
    void checkClientsCount();
    void keepTrackOfClientsKilled();
};

#endif // TEST_RESOURCEBOOKKEEPER_H
