#ifndef SourceScanner_H
#define SourceScanner_H

#include "io-device/iodevicebrokendummy.h"
#include "io-device/iotransferdatasingle.h"
#include "source-device/sourcedevicevein.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>
#include <QUuid>
#include <QSharedPointer>

class IoDeviceBrokenDummy;

class SourceScanner : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceScanner> Ptr;
    static Ptr createScanner(IoDeviceBase::Ptr ioDevice, QUuid uuid);
    virtual ~SourceScanner();

    // requests
    void startScan();

    // getter
    SourceDeviceVein* getSourceDeviceFound();
    QUuid getUuid();
    static int getInstanceCount();
signals:
    void sigScanFinished(SourceScanner::Ptr scanner);

private slots:
    void onIoFinished(int ioId, bool ioDeviceError);
private:
    explicit SourceScanner(IoDeviceBase::Ptr ioDevice, QUuid uuid);

    void sendReceiveSourceID();
    QByteArray createInterfaceSpecificPrepend();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);
    QByteArray extractNameFromResponse(QByteArray responsePrefix, QByteArray version, SupportedSourceTypes sourceType);
    static SupportedSourceTypes nextDemoType();

    IoIdKeeper m_currIoId;
    IoDeviceBase::Ptr m_ioDevice;
    QUuid m_uuid;
    SourceDeviceVein* m_sourceDeviceFound = nullptr;
    IoTransferDataSingle::Ptr m_ioDataSingle = nullptr;
    int m_currentSourceTested = 0;
    SourceScanner::Ptr m_safePoinerOnThis;
    static int m_InstanceCount;
};

#endif // SourceScanner_H
