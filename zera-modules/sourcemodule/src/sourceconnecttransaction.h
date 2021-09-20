#ifndef CSOURCECONNECTTRANSACTION_H
#define CSOURCECONNECTTRANSACTION_H

#include <QObject>
#include <QTimer>

class cIOInterface;

namespace SOURCEMODULE
{
class cSourceDevice;


class cSourceConnectTransaction : public QObject
{
    Q_OBJECT
public:
    explicit cSourceConnectTransaction(cIOInterface *interface, QObject *parent = nullptr);
    cSourceDevice* sourceDeviceFound();
    void startConnect();

private:
    cIOInterface* m_IOInterface;
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
