#ifndef CSOURCEDEVICEMANAGER_H
#define CSOURCEDEVICEMANAGER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "sourcedevicetemplate.h"
#include "sourcescanner.h"

class SourceDeviceManager : public QObject
{
    Q_OBJECT
public:
    SourceDeviceManager(int countSlots, QObject *parent = 0);
    void addInternalSource(QString sourceJsonStruct);
    void startSourceScan(const IoDeviceTypes ioDeviceType, const QString deviceInfo, const QUuid uuid);
    void closeSource(int slotNo, const QUuid uuid);
    void closeSource(QString ioDeviceInfo, const QUuid uuid);
    void closeAll();
    void setDemoCount(int count);

    int getSlotCount();
    int getActiveSlotCount();
    SourceDeviceTemplate::Ptr getSourceController(int slotNo);
    int getDemoCount();
signals:
    void sigSourceScanFinished(int slotNo, QUuid uuid, QString errorMsg);
    void sigSlotRemoved(int slotNo, QUuid uuid, QString errorMsg);
    void sigAllSlotsRemoved();

private slots:
    void onScanFinished(SourceScanner::Ptr scanner);
    void onSourceClosed(int facadeId, QUuid uuid);
signals:
    void sigSlotRemovedQueued(int slotNo, QUuid uuid, QString errorMsg);
private:
    bool isValidSlotNo(int slotNo);
    int findFreeSlot();
    int tryAddSourceToFreeSlot(IoDeviceBase::Ptr ioDevice, SourceProperties props);
    void addSource(int slotPos, SourceDeviceTemplate::Ptr deviceController);
    bool tryStartDemoDeviceRemove(int slotNo);
    void checkHandleAllClosed();
    QVector<SourceDeviceTemplate::Ptr> m_sourceControllers;
    QVector<bool> m_activeSlots;
};

#endif // CSOURCEDEVICEMANAGER_H
