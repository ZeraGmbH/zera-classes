#ifndef CSOURCEDEVICEMANAGER_H
#define CSOURCEDEVICEMANAGER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "sourceinterface.h"
#include "sourcescanner.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{

class cSourceDevice;

class cSourceDeviceManager : public QObject
{
    Q_OBJECT
public:
    cSourceDeviceManager(int countSlots, QObject *parent = 0);

    // requests
    void startSourceScan(const SourceInterfaceTypes interfaceType, const QString deviceInfo, const QUuid uuid);
    void closeSource(int slotNo, const QUuid uuid);
    void closeSource(QString interfaceDeviceInfo, const QUuid uuid);
    void setDemoCount(int count);
    void setDemoFollowsTimeout(bool follow);

    // getter
    int getSlotCount();
    int getActiveSlotCount();
    cSourceDevice* getSourceDevice(int slotNo);
    int getDemoCount();

signals:
    void sigSourceScanFinished(int slotNo, cSourceDevice* device, QUuid uuid, QString errMsg);
    void sigSlotRemoved(int slotNo, QUuid uuid, QString errMsg);

private slots:
    void onScanFinished(tSourceScannerShPtr scanner);
    void onSourceClosed(cSourceDevice *sourceDevice, QUuid uuid);

signals:
    void sigSlotRemovedQueued(int slotNo, QUuid uuid, QString errMsg);

private:
    bool isValidSlotNo(int slotNo);
    int findFreeSlot();
    void addSource(int slotPos, cSourceDevice *device);
    bool tryStartDemoDeviceRemove(int slotNo);

    QVector<cSourceDevice*> m_sourceDeviceSlots;
    QVector<QUuid> m_PendingRemoveHashes;
    int m_activeSlotCount = 0;

    bool m_bDemoDelayFollowsTimeout = false;
};

}

#endif // CSOURCEDEVICEMANAGER_H
