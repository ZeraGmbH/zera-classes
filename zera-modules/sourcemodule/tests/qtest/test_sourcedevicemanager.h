#ifndef SOURCEDEVICEMANAGER_H
#define SOURCEDEVICEMANAGER_H

#include <QObject>
#include "source-device/sourcedevicemanager.h"

struct FinishEntry
{
    FinishEntry(int slotNo, QUuid uuid, QString errMsg);
    int slotNo;
    QUuid uuid;
    QString errMsg;
};

class test_sourcedevicemanager : public QObject
{
    Q_OBJECT
public slots:
    void onSourceScanFinished(int slotNo, QUuid uuid, QString errMsg);
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

    void removeAllSignalOnEmpty();
    void removeAllSignalOnSlots();

    void demoScanOne();
    void demoScanTooMany();

    void demoScanAll();
    void demoScanAllAndRemove();

    void demoAddNotification();
    void demoAddNotificationTooMany();

    void demoRemoveNotification();
    void demoRemoveNotificationTooMany();

    void demoScanNoDevFoundOnBroken();
    void demoScanNoDevFoundOnDemo();

private:
    void checkSlotCount(SourceDeviceManager &devMan, int total, int active, int demo);
    void checkAddRemoveNotifications(int total, int add, int remove);

    QList<FinishEntry> m_listSourcesAdded;
    QList<int> m_socketsRemoved;
};

#endif // SOURCEDEVICEMANAGER_H
