#ifndef CSOURCECONNECTTRANSACTION_H
#define CSOURCECONNECTTRANSACTION_H

#include <QObject>
#include <QTimer>

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceDevice;


class cSourceConnectTransaction : public QObject
{
    Q_OBJECT
public:
    explicit cSourceConnectTransaction(cSourceInterfaceBase *interface, QObject *parent = nullptr);

    // requests
    void startConnect();

    // getter
    cSourceDevice* sourceDeviceFound();

private:
    cSourceInterfaceBase* m_IOInterface;
    cSourceDevice* m_sourceDeviceIdentified;

    QTimer m_demoTimer;
    // TODO: Info on fail data

signals:
    void sigTransactionFinished(cSourceConnectTransaction* transaction);

private slots:
    void demoConnectTimeout();

};

}
#endif // CSOURCECONNECTTRANSACTION_H
