#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QSharedPointer>
#include "sourcedevicestatus.h"
#include "sourceiotransactiongenerator.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceVeinInterface;

class cSourceDevice : public QObject
{
    Q_OBJECT
public:
    enum SourceType { // TODO replace by protocol
        SOURCE_DEMO = 0,
        SOURCE_MT3000,   // basic
        SOURCE_MT400_20, // current only
        SOURCE_MT507,    // single phase

        SOURCE_DEMO_FG_4PHASE, // AUX/harmonics

        SOURCE_TYPE_COUNT
    };
    explicit cSourceDevice(QSharedPointer<cSourceInterfaceBase> interface, SourceType type, QString version);
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
    QString getDeviceStructureFileName();
    const QJsonObject getOrLoadDeviceParamStructure();
    QString getStateFileName();
    const QJsonObject loadDeviceParamState();

    QSharedPointer<cSourceInterfaceBase> m_spIoInterface;

    QJsonObject m_jsonParamsStructure;
    QJsonObject m_currParamState;
    QJsonObject m_requestedParamState;

    cSourceIoTransactionGenerator m_ioTransactionGenerator;

    cSourceDeviceStatus  m_deviceStatus;
    cSourceVeinInterface* m_veinInterface = nullptr;

    SourceType m_type;
    QString m_version;

    SourceType m_demoType;
    QTimer m_demoOnOffDeleayTimer;
};

}
#endif // CSOURCEDEVICE_H
