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
    void setDemoCount(int count);
    bool closeSource(int slotNo);

    // getter
    int getSlotCount();
    int getActiveSlotCount();
    int getDemoCount();
    cSourceDevice* getSourceDevice(int slotNo);

signals:
    void sigSourceScanFinished(int slotNo, cSourceDevice* device, QUuid uuid, QString errMsg);
    void sigSlotRemoved(int slotNo);

private slots:
    void onScanFinished(tSourceScannerShPtr scanner);
    void onSourceClosed(cSourceDevice *sourceDevice);

signals:
    void sigSlotRemovedQueued(int slotNo);

private:
    bool isValidSlotNo(int slotNo);
    int findFreeSlot();
    void addSource(int slotPos, cSourceDevice *device);
    bool tryStartDemoDeviceRemove(int slotNo);

    QVector<cSourceDevice*> m_sourceDeviceSlots;
    int m_activeSlotCount = 0;
};

}

#endif // CSOURCEDEVICEMANAGER_H
