#ifndef cSourceScanner_H
#define cSourceScanner_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include "sourcedevice.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceDevice;

class cSourceScanner : public QObject
{
    Q_OBJECT
public:
    explicit cSourceScanner(QSharedPointer<cSourceInterfaceBase> interface, QUuid uuid);
    virtual ~cSourceScanner();
    // requests
    void startScan();
    // getter
    cSourceDevice* getSourceDeviceFound();
    QUuid getUuid();
    // keep alive ensurance
    void setScannerReference(QSharedPointer<cSourceScanner> scannerReference);
signals:
    void sigScanFinished(QSharedPointer<cSourceScanner> scanner);
private slots:
    void onIoFinished(int ioID);
private:
    void sendReceiveSourceID();
    QByteArray extractVersionFromResponse(SupportedSourceTypes sourceType);

    QSharedPointer<cSourceInterfaceBase> m_ioInterface;
    QUuid m_uuid;
    cSourceDevice* m_sourceDeviceIdentified;
    QByteArray m_bytesReceived;
    int m_currentSourceTested = 0;
    QSharedPointer<cSourceScanner> m_scannerReference;
};

}
#endif // cSourceScanner_H
