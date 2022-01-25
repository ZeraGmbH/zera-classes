#ifndef SOURCEDEVICESCANNER_H
#define SOURCEDEVICESCANNER_H

#include "io-device/iotransferdatasingle.h"
#include "source-device/sourcedevicevein.h"
#include "transaction-ids/idkeeper.h"

#include <QObject>
#include <QUuid>
#include <QSharedPointer>

class SourceDeviceScanner : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceDeviceScanner> Ptr;
    static Ptr createScanner(IoDeviceBase::Ptr ioDevice, QUuid uuid);
    virtual ~SourceDeviceScanner();

    // requests
    void startScan();

    // getter
    SourceDeviceVein* getSourceDeviceFound();
    QUuid getUuid();
    static int getInstanceCount();
signals:
    void sigScanFinished(SourceDeviceScanner::Ptr scanner);

private slots:
    void onIoFinished(int ioId, bool ioDeviceError);
private:
    explicit SourceDeviceScanner(IoDeviceBase::Ptr ioDevice, QUuid uuid);

    void sendReceiveSourceID();
    QByteArray createIoDeviceSpecificPrepend();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);
    QByteArray extractNameFromResponse(QByteArray responsePrefix, QByteArray version, SupportedSourceTypes sourceType);
    static SupportedSourceTypes nextDemoType();

    IdKeeperSingle m_currIoId;
    IoDeviceBase::Ptr m_ioDevice;
    QUuid m_uuid;
    SourceDeviceVein* m_sourceDeviceFound = nullptr;
    IoTransferDataSingle::Ptr m_ioDataSingle = nullptr;
    int m_currentSourceTested = 0;
    SourceDeviceScanner::Ptr m_safePoinerOnThis;
    static int m_InstanceCount;
};

#endif // SOURCEDEVICESCANNER_H
