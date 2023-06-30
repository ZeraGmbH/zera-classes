#include "test_resourcebookkeeper.h"
#include "resourcebookkeeper.h"
#include "resourceclient.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_resourcebookkeeper)

void test_resourcebookkeeper::initialAmountTotal()
{
    ResourceBookkeeper keeper(42);
    QCOMPARE(keeper.getTotal(), 42);
}

void test_resourcebookkeeper::initialAmountAllocated()
{
    ResourceBookkeeper keeper(42);
    QCOMPARE(keeper.getAllocated(), 0);
}

void test_resourcebookkeeper::initialAmountFree()
{
    ResourceBookkeeper keeper(42);
    QCOMPARE(keeper.getFree(), 42);
}

void test_resourcebookkeeper::singleAlloc()
{
    ResourceBookkeeper keeper(42);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    QVERIFY(client->alloc(7));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(keeper.getAllocated(), 7);
    QCOMPARE(keeper.getFree(), 42-7);
    QCOMPARE(client->getAllocated(), 7);
}

void test_resourcebookkeeper::allocTooMuchDenied()
{
    ResourceBookkeeper keeper(5);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    QVERIFY(!client->alloc(7));
    QCOMPARE(spy.count(), 0);
    QCOMPARE(keeper.getAllocated(), 0);
    QCOMPARE(client->getAllocated(), 0);
}

void test_resourcebookkeeper::negAllocDenied()
{
    ResourceBookkeeper keeper(5);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    QCOMPARE(spy.count(), 0);
    QVERIFY(!client->alloc(-1));
}

void test_resourcebookkeeper::twoAllocsOneClient()
{
    ResourceBookkeeper keeper(10);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    QVERIFY(client->alloc(2));
    QCOMPARE(spy.count(), 1);
    QVERIFY(client->alloc(2));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(keeper.getAllocated(), 4);
    QCOMPARE(client->getAllocated(), 4);
}

void test_resourcebookkeeper::twoAllocsTwoClients()
{
    ResourceBookkeeper keeper(10);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client1 = keeper.registerClient();
    ResourceClientPtr client2 = keeper.registerClient();
    QVERIFY(client1->alloc(2));
    QCOMPARE(spy.count(), 1);
    QVERIFY(client2->alloc(2));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(keeper.getAllocated(), 4);
    QCOMPARE(client1->getAllocated(), 2);
    QCOMPARE(client2->getAllocated(), 2);
}

void test_resourcebookkeeper::allocAndFree()
{
    ResourceBookkeeper keeper(10);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    QVERIFY(client->alloc(4));
    QCOMPARE(spy.count(), 1);
    client->freeAll();
    QCOMPARE(spy.count(), 2);
    QCOMPARE(keeper.getAllocated(), 0);
    QCOMPARE(client->getAllocated(), 0);
}

void test_resourcebookkeeper::onInitialFreeFiresNoSignal()
{
    ResourceBookkeeper keeper(10);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    client->freeAll();
    QCOMPARE(spy.count(), 0);
}

void test_resourcebookkeeper::allocFreeTwice()
{
    ResourceBookkeeper keeper(10);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client = keeper.registerClient();
    QVERIFY(client->alloc(4));
    QCOMPARE(spy.count(), 1);
    client->freeAll();
    QCOMPARE(spy.count(), 2);
    client->freeAll();
    QCOMPARE(spy.count(), 2);
    QCOMPARE(keeper.getAllocated(), 0);
    QCOMPARE(client->getAllocated(), 0);
}

void test_resourcebookkeeper::killingKeeperFreesAllClients()
{
    ResourceBookkeeper *keeper = new ResourceBookkeeper(10);
    ResourceClientPtr client1 = keeper->registerClient();
    ResourceClientPtr client2 = keeper->registerClient();
    client1->alloc(4);
    QCOMPARE(client1->getAllocated(), 4);
    client2->alloc(4);
    QCOMPARE(client2->getAllocated(), 4);
    delete keeper;
    QCOMPARE(client1->getAllocated(), 0);
    QCOMPARE(client2->getAllocated(), 0);
}

void test_resourcebookkeeper::noAllocsOnDeadKeeper()
{
    ResourceBookkeeper *keeper = new ResourceBookkeeper(10);
    ResourceClientPtr client = keeper->registerClient();
    delete keeper;
    QVERIFY(!client->alloc(4));
}

void test_resourcebookkeeper::checkClientsCount()
{
    ResourceBookkeeper keeper(10);
    QCOMPARE(keeper.getClientCount(), 0);
    ResourceClientPtr client1 = keeper.registerClient();
    QCOMPARE(keeper.getClientCount(), 1);
    ResourceClientPtr client2 = keeper.registerClient();
    QCOMPARE(keeper.getClientCount(), 2);
}

void test_resourcebookkeeper::keepTrackOfClientsKilled()
{
    ResourceBookkeeper keeper(10);
    QSignalSpy spy(&keeper, &ResourceBookkeeper::sigAllocationChanged);
    ResourceClientPtr client1 = keeper.registerClient();
    client1->alloc(2);
    ResourceClientPtr client2 = keeper.registerClient();
    client2->alloc(2);
    ResourceClientPtr client3 = keeper.registerClient();
    client3->alloc(2);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(keeper.getClientCount(), 3);
    QCOMPARE(keeper.getAllocated(), 6);
    client1 = nullptr;
    QCOMPARE(spy.count(), 4);
    QCOMPARE(keeper.getClientCount(), 2);
    QCOMPARE(keeper.getAllocated(), 4);
    client2 = nullptr;
    QCOMPARE(spy.count(), 5);
    QCOMPARE(keeper.getClientCount(), 1);
    QCOMPARE(keeper.getAllocated(), 2);
    client3 = nullptr;
    QCOMPARE(spy.count(), 6);
    QCOMPARE(keeper.getClientCount(), 0);
    QCOMPARE(keeper.getAllocated(), 0);
}
