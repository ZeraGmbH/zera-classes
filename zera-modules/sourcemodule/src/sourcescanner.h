#ifndef cSourceScanner_H
#define cSourceScanner_H

#include <QObject>
#include <QUuid>

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceDevice;

class cSourceScanner : public QObject
{
    Q_OBJECT
public:
    explicit cSourceScanner(cSourceInterfaceBase *interface, QUuid uuid, QObject *parent = nullptr);
    // requests
    void startScan();
    // getter
    cSourceDevice* sourceDeviceFound();
    QUuid getUuid();
signals:
    void sigTransactionFinished(cSourceScanner* transaction);
private:
    void sendReceiveSourceID();

    cSourceInterfaceBase* m_IOInterface;
    QUuid m_uuid;
    cSourceDevice* m_sourceDeviceIdentified;
    QByteArray m_receivedData;
    int m_currentSourceTested = 0;
private slots:
    void onIoFinished(int transactionID);
};

}
#endif // cSourceScanner_H
