#ifndef SOURCEDEVICEMANAGER_H
#define SOURCEDEVICEMANAGER_H

#include <QObject>
#include "sourcedevice.h"

using SOURCEMODULE::cSourceDevice;

struct FinishEntry
{
    FinishEntry(int slotNo, cSourceDevice* device, QUuid uuid, QString errMsg);
    int slotNo;
    cSourceDevice* device;
    QUuid uuid;
    QString errMsg;
};

class SourceDeviceManagerTest : public QObject
{
    Q_OBJECT
public slots:
    void onSourceScanFinished(int slotNo, cSourceDevice* device, QUuid uuid, QString errMsg);
    void onSlotRemoved(int slotNo);

private slots:
    void init();

    void testInitState();
    void testAddTooMany();
    void testRemoveOnEmpty();
    void testRemoveInvalidBelow();
    void testRemoveInvalidAbove();
    void testRemoveTooMany();

    void testAvailAndDemoCountAdd();
    void testAvailAndDemoCountAddRemove();

    void testAddNotification();
    void testAddNotificationTooMany();

    void testRemoveNotification();
    void testRemoveNotificationTooMany();
    void testAddRemoveNotificationAndCounts();

    void testNoCrashOnManagerDeadBeforeScanFinished();

private:
    QList<FinishEntry> m_listSourcesAdded;
    QList<int> m_socketsRemoved;
};

#endif // SOURCEDEVICEMANAGER_H
