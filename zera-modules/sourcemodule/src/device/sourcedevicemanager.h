#ifndef CSOURCEDEVICEMANAGER_H
#define CSOURCEDEVICEMANAGER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "sourcescanner.h"
#include "io-interface/sourceinterface.h"

class SourceInterfaceBase;
class SourceDeviceVein;

class SourceDeviceManager : public QObject
{
    Q_OBJECT
public:
    SourceDeviceManager(int countSlots, QObject *parent = 0);

    // requests
    void startSourceScan(const SourceInterfaceTypes interfaceType, const QString deviceInfo, const QUuid uuid);
    void closeSource(int slotNo, const QUuid uuid);
    void closeSource(QString interfaceDeviceInfo, const QUuid uuid);
    void setDemoCount(int count);
    void setDemoFollowsTimeout(bool follow);

    // getter
    int getSlotCount();
    int getActiveSlotCount();
    SourceDeviceVein* getSourceDevice(int slotNo);
    int getDemoCount();

signals:
    void sigSourceScanFinished(int slotNo, SourceDeviceVein* device, QUuid uuid, QString errMsg);
    void sigSlotRemoved(int slotNo, QUuid uuid, QString errMsg);

private slots:
    void onScanFinished(tSourceScannerShPtr scanner);
    void onSourceClosed(SourceDeviceVein *sourceDevice, QUuid uuid);

signals:
    void sigSlotRemovedQueued(int slotNo, QUuid uuid, QString errMsg);

private:
    bool isValidSlotNo(int slotNo);
    int findFreeSlot();
    void addSource(int slotPos, SourceDeviceVein *device);
    bool tryStartDemoDeviceRemove(int slotNo);

    QVector<SourceDeviceVein*> m_sourceDeviceSlots;
    QVector<QUuid> m_PendingRemoveHashes;
    int m_activeSlotCount = 0;

    bool m_bDemoDelayFollowsTimeout = false;
};

#endif // CSOURCEDEVICEMANAGER_H
