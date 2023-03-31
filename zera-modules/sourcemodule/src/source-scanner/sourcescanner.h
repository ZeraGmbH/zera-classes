#ifndef SOURCESCANNER_H
#define SOURCESCANNER_H

#include "sourcescannerstrategy.h"
#include "sourceprotocols.h"
#include "sourceio.h"
#include "sourceproperties.h"
#include "iodevicetypes.h"
#include "idkeeper.h"
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
    void onTransferGroupFinished(IoQueueGroup::Ptr transferGroup);
private:
    void finishScan();

    ISourceScannerStrategy::Ptr m_ioStrategy;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;
    IoQueueGroupListPtr m_ioQueueList;
    SourceProperties m_SourcePropertiesFound;
    QUuid m_uuid;
    IdKeeperMulti m_pendingQueueEntries;
signals:
    void sigScanFinishedQueued(SourceScanner::Ptr scanner);
};

#endif // SOURCESCANNER_H
