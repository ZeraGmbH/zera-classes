#ifndef CSOURCEDEVICEMANAGER_H
#define CSOURCEDEVICEMANAGER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "sourcedevicefacade.h"
#include "source-scanner/sourcescanner.h"

class SourceDeviceManager : public QObject
{
    Q_OBJECT
public:
    SourceDeviceManager(int countSlots, QObject *parent = 0);
    void startSourceScan(const IoDeviceTypes ioDeviceType, const QString deviceInfo, const QUuid uuid);
    void closeSource(int slotNo, const QUuid uuid);
    void closeSource(QString ioDeviceInfo, const QUuid uuid);
    void setDemoCount(int count);

    int getSlotCount();
    int getActiveSlotCount();
    SourceDeviceFacade* getSourceController(int slotNo);
    int getDemoCount();
signals:
    void sigSourceScanFinished(int slotNo, QUuid uuid, QString errMsg);
    void sigSlotRemoved(int slotNo, QUuid uuid, QString errMsg);

private slots:
    void onScanFinished(SourceScanner::Ptr scanner);
    void onSourceClosed(SourceDeviceFacade *source, QUuid uuid);
signals:
    void sigSlotRemovedQueued(int slotNo, QUuid uuid, QString errMsg);
private:
    bool isValidSlotNo(int slotNo);
    int findFreeSlot();
    void addSource(int slotPos, SourceDeviceFacade *deviceController);
    bool tryStartDemoDeviceRemove(int slotNo);
    QVector<SourceDeviceFacade*> m_sourceControllers;
    QVector<QUuid> m_PendingRemoveHashes;
    int m_activeSlotCount = 0;
};

#endif // CSOURCEDEVICEMANAGER_H
