#ifndef CSOURCEDEVICEMANAGER_H
#define CSOURCEDEVICEMANAGER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "sourceinterface.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{

class cSourceDevice;
class cSourceScanner;

class cSourceDeviceManager : public QObject
{
    Q_OBJECT
public:
    cSourceDeviceManager(int countSlots, QObject *parent = 0);

    // requests
    void startSourceScan(const SourceInterfaceType interfaceType,  const QString deviceInfo, const QUuid uuid);

    // getter
    int activeSlotCount();
    cSourceDevice* sourceDevice(int slotNo);

signals:
    void sigSourceScanFinished(int slotNo, cSourceDevice* device, QUuid uuid, QString errMsg);
    void sigSlotRemoved(int slotNo);

private slots:
    void onScanFinished(QSharedPointer<cSourceScanner> transaction);
    void onRemoveSource(cSourceDevice *sourceDevice);

private:
    QVector<cSourceDevice*> m_sourceDeviceSlots;
    int m_activeSlotCount;
};

}

#endif // CSOURCEDEVICEMANAGER_H
