#ifndef SOURCESCANNER_H
#define SOURCESCANNER_H

#include "sourcescannertemplate.h"
#include "sourceproperties.h"
#include "iodevicebase.h"
#include "ioqueue.h"
#include <QObject>
#include <QUuid>

class SourceScanner : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceScanner> Ptr;
    static Ptr create(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid=QUuid());

    void startScan();

    SourceProperties getSourcePropertiesFound() const;
    IoDeviceBase::Ptr getIoDevice();
    QUuid getUuid() const;
signals:
    void sigScanFinished(SourceScanner::Ptr scanner);

protected:
    SourceScanner(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid);
    Ptr m_safePoinerOnThis;
private slots:
    void onTransferGroupFinished(IoQueueGroup::Ptr transferGroup);
private:
    void startNextScan();
    void finishScan();

    SourceScannerTemplate::Ptr m_ioStrategy;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;
    IoQueueGroupListPtr m_ioQueueList;
    SourceProperties m_SourcePropertiesFound;
    QUuid m_uuid;
signals:
    void sigScanFinishedQueued(SourceScanner::Ptr scanner);
};

#endif // SOURCESCANNER_H
