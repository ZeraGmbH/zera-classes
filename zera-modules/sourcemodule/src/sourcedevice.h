#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QSharedPointer>
#include "sourcedevicestatus.h"
#include "sourceiotransactiongenerator.h"
#include "supportedsources.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceVeinInterface;

class cSourceDevice : public QObject
{
    Q_OBJECT
public:
    explicit cSourceDevice(QSharedPointer<cSourceInterfaceBase> interface, SupportedSourceTypes type, QString version);
    virtual ~cSourceDevice();

    // requests
    void close();

    // getter
    QSharedPointer<cSourceInterfaceBase> getIoInterface();
    bool isDemo();

    // setter
    void setVeinInterface(cSourceVeinInterface* veinInterface);
    void startActions(tSourceIoTransactionList transactionList);
    void saveState(); // persistency

signals:
    void sigClosed(cSourceDevice* getSourceDevice);

public slots:
    void onNewVeinParamStatus(QVariant paramState);
    void onTimeoutDemoTransaction();

private slots:
    void onInterfaceClosed();

private:
    QSharedPointer<cSourceInterfaceBase> m_spIoInterface;

    cSourceJsonStateIo* m_paramStateIo;
    QJsonObject m_currParamState;
    QJsonObject m_requestedParamState;

    cSourceIoTransactionGenerator m_ioTransactionGenerator;

    cSourceDeviceStatus  m_deviceStatus;
    cSourceVeinInterface* m_veinInterface = nullptr;

    SupportedSourceTypes m_type;
    QString m_version;

    QTimer m_demoOnOffDelayTimer;
};

}
#endif // CSOURCEDEVICE_H
