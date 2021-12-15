#ifndef cSourceScanner_H
#define cSourceScanner_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include "sourceinterface.h"
#include "sourcedevicevein.h"

class SourceInterfaceBase;

namespace SOURCEMODULE
{
class SourceDeviceVein;
class cSourceScanner;
typedef QSharedPointer<cSourceScanner> tSourceScannerShPtr;


class cSourceScanner : public QObject
{
    Q_OBJECT
public:
    static tSourceScannerShPtr createScanner(tSourceInterfaceShPtr interface, QUuid uuid);
    virtual ~cSourceScanner();

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
    explicit cSourceScanner(tSourceInterfaceShPtr interface, QUuid uuid);

    void sendReceiveSourceID();
    QByteArray createInterfaceSpecificPrepend();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);
    QByteArray extractNameFromResponse(QByteArray responsePrefix, QByteArray version, SupportedSourceTypes sourceType);
    static SupportedSourceTypes nextDemoType();

    int m_ioId = 0;
    tSourceInterfaceShPtr m_ioInterface;
    QUuid m_uuid;
    SourceDeviceVein* m_sourceDeviceIdentified;
    QByteArray m_bytesReceived;
    int m_currentSourceTested = 0;
    tSourceScannerShPtr m_safePoinerOnThis;
    static int m_InstanceCount;
};

}
#endif // cSourceScanner_H
