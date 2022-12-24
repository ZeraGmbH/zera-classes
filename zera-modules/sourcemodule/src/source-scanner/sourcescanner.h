#ifndef SOURCESCANNER_H
#define SOURCESCANNER_H

#include "sourcescannerstrategy.h"
#include "source-protocols/sourceprotocols.h"
#include "source-device/sourceio.h"
#include "source-device/sourceproperties.h"
#include "iodevicetypes.h"
#include "transaction-ids/idkeeper.h"
#include <QObject>
#include <QUuid>

class SourceScanner : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceScanner> Ptr;
    static Ptr create(IoDeviceBase::Ptr ioDevice, ISourceScannerStrategy::Ptr ioStrategy, QUuid uuid=QUuid());

    void startScan();

    SourceProperties getSourcePropertiesFound() const;
    IoDeviceBase::Ptr getIoDevice();
    QUuid getUuid() const;
signals:
    void sigScanFinished(SourceScanner::Ptr scanner);

protected:
    SourceScanner(IoDeviceBase::Ptr ioDevice, ISourceScannerStrategy::Ptr ioStrategy, QUuid uuid);
    Ptr m_safePoinerOnThis;
private slots:
    void onTransferGroupFinished(IoQueueEntry::Ptr transferGroup);
private:
    void finishScan();

    ISourceScannerStrategy::Ptr m_ioStrategy;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;
    IoQueueEntryList m_ioQueueList;
    SourceProperties m_SourcePropertiesFound;
    QUuid m_uuid;
    IdKeeperMulti m_pendingQueueEntries;
signals:
    void sigScanFinishedQueued(SourceScanner::Ptr scanner);
};

#endif // SOURCESCANNER_H
