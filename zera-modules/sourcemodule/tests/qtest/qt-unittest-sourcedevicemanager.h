#ifndef SOURCEDEVICEMANAGER_H
#define SOURCEDEVICEMANAGER_H

#include <QObject>
#include "source-device/sourcedevicevein.h"
#include "source-device/sourcedevicemanager.h"

struct FinishEntry
{
    FinishEntry(int slotNo, SourceDeviceVein* device, QUuid uuid, QString errMsg);
    int slotNo;
    SourceDeviceVein* device;
    QUuid uuid;
    QString errMsg;
};

class SourceDeviceManagerTest : public QObject
{
    Q_OBJECT
public slots:
    void onSourceScanFinished(int slotNo, SourceDeviceVein* device, QUuid uuid, QString errMsg);
    void onSlotRemoved(int slotNo);

private slots:
    void init();

    void initSlotCount();
    void removeSlotsOnEmpty();
    void removeSlotInvalidBelow();
    void removeSlotInvalidAbove();
    void removeTooManySlots();
    void removeDevInfoUnknown();
    void removeDevInfoUuidIdentical();

    void demoScanOne();
    void demoScanTooMany();

    void demoScanAll();
    void demoScanAllAndRemove();

    void demoAddNotification();
    void demoAddNotificationTooMany();

    void demoRemoveNotification();
    void demoRemoveNotificationTooMany();

private:
    void checkSlotCount(SourceDeviceManager &devMan, int total, int active, int demo);
    void checkAddRemoveNotifications(int total, int add, int remove);

    QList<FinishEntry> m_listSourcesAdded;
    QList<int> m_socketsRemoved;
};

#endif // SOURCEDEVICEMANAGER_H
