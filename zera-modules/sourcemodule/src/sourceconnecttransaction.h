#ifndef CSOURCECONNECTTRANSACTION_H
#define CSOURCECONNECTTRANSACTION_H

#include <QObject>
#include <QUuid>

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceDevice;

class cSourceConnectTransaction : public QObject
{
    Q_OBJECT
public:
    explicit cSourceConnectTransaction(cSourceInterfaceBase *interface, QUuid uuid, QObject *parent = nullptr);
    // requests
    void startScan();
    // getter
    cSourceDevice* sourceDeviceFound();
    QUuid getUuid();
signals:
    void sigTransactionFinished(cSourceConnectTransaction* transaction);
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
#endif // CSOURCECONNECTTRANSACTION_H
