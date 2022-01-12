#ifndef SourceScanner_H
#define SourceScanner_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include "sourceinterface.h"
#include "device/sourcedevicevein.h"
#include "io-ids/sourceidkeeper.h"

class SourceInterfaceBase;

class SourceDeviceVein;
class SourceScanner;
typedef QSharedPointer<SourceScanner> tSourceScannerShPtr;

class SourceScanner : public QObject
{
    Q_OBJECT
public:
    static tSourceScannerShPtr createScanner(tSourceInterfaceShPtr interface, QUuid uuid);
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
    void onIoFinished(int ioId, bool error);
private:
    explicit SourceScanner(tSourceInterfaceShPtr interface, QUuid uuid);

    void sendReceiveSourceID();
    QByteArray createInterfaceSpecificPrepend();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);
    QByteArray extractNameFromResponse(QByteArray responsePrefix, QByteArray version, SupportedSourceTypes sourceType);
    static SupportedSourceTypes nextDemoType();

    SourceIdKeeper m_currIoId;
    tSourceInterfaceShPtr m_ioInterface;
    QUuid m_uuid;
    SourceDeviceVein* m_sourceDeviceIdentified;
    QByteArray m_bytesReceived;
    int m_currentSourceTested = 0;
    tSourceScannerShPtr m_safePoinerOnThis;
    static int m_InstanceCount;
};

#endif // SourceScanner_H
