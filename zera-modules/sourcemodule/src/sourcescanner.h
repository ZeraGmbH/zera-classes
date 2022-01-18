#ifndef SourceScanner_H
#define SourceScanner_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include "io-device/iodevicebaseserial.h"
#include "source-device/sourcedevicevein.h"
#include "io-ids/ioidkeeper.h"

class IODeviceBaseSerial;

class SourceDeviceVein;
class SourceScanner;
typedef QSharedPointer<SourceScanner> tSourceScannerShPtr;

class SourceScanner : public QObject
{
    Q_OBJECT
public:
    static tSourceScannerShPtr createScanner(tIoDeviceShPtr interface, QUuid uuid);
    virtual ~SourceScanner();

    // requests
    void startScan();

    // getter
    SourceDeviceVein* getSourceDeviceFound();
    QUuid getUuid();
    static int getInstanceCount();
signals:
    void sigScanFinished(tSourceScannerShPtr scanner);

private slots:
    void onIoFinished(int ioId, bool ioDeviceError);
private:
    explicit SourceScanner(tIoDeviceShPtr interface, QUuid uuid);

    void sendReceiveSourceID();
    QByteArray createInterfaceSpecificPrepend();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);
    QByteArray extractNameFromResponse(QByteArray responsePrefix, QByteArray version, SupportedSourceTypes sourceType);
    static SupportedSourceTypes nextDemoType();

    IoIdKeeper m_currIoId;
    tIoDeviceShPtr m_ioInterface;
    QUuid m_uuid;
    SourceDeviceVein* m_sourceDeviceIdentified;
    QByteArray m_bytesReceived;
    int m_currentSourceTested = 0;
    tSourceScannerShPtr m_safePoinerOnThis;
    static int m_InstanceCount;
};

#endif // SourceScanner_H
