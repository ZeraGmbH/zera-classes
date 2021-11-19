#ifndef cSourceScanner_H
#define cSourceScanner_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include "sourceinterface.h"
#include "sourcedevice.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceDevice;
class cSourceScanner;
typedef QSharedPointer<cSourceScanner> tSourceScannerShPtr;


class cSourceScanner : public QObject
{
    Q_OBJECT
public:
    explicit cSourceScanner(tSourceInterfaceShPtr interface, QUuid uuid);
    virtual ~cSourceScanner();
    // requests
    void startScan();
    // getter
    cSourceDevice* getSourceDeviceFound();
    QUuid getUuid();
    // keep alive ensurance
    void setScannerReference(tSourceScannerShPtr scannerReference);
signals:
    void sigScanFinished(tSourceScannerShPtr scanner);
private slots:
    void onIoFinished(int ioID);
private:
    void sendReceiveSourceID();
    QByteArray createInterfaceSpecificPrepend();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);

    tSourceInterfaceShPtr m_ioInterface;
    QUuid m_uuid;
    cSourceDevice* m_sourceDeviceIdentified;
    QByteArray m_bytesReceived;
    int m_currentSourceTested = 0;
    tSourceScannerShPtr m_scannerReference;
};

}
#endif // cSourceScanner_H
