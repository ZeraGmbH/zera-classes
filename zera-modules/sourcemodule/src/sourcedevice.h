#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include "sourcedevicestatus.h"

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
    explicit cSourceDevice(cSourceInterfaceBase* interface, SourceType type, QObject *parent = nullptr);
    virtual ~cSourceDevice();

    // requests
    void close();

    // getter
    cSourceInterfaceBase* ioInterface();

    // setter
    void setVeinInterface(cSourceVeinInterface* veinInterface);

    void saveState();

signals:
    void sigClosed(cSourceDevice* sourceDevice);

public slots:
    void newVeinParamStatus(QVariant paramState);
    void timeoutDemoTransaction();

private slots:
    void onInterfaceClosed(cSourceInterfaceBase *ioInterface);

private:
    QString deviceFileName();
    const QJsonObject deviceParamStructure();
    const QJsonObject deviceParamState();
    QString stateFileName();

    cSourceInterfaceBase* m_IOInterface = nullptr; // WE own the interface

    QJsonObject m_jsonParamsStructure;
    QJsonObject m_currParamState;
    QJsonObject m_requestedParamState;

    cSourceDeviceStatus  m_deviceStatus;
    cSourceVeinInterface* m_veinInterface = nullptr;

    SourceType m_type;

    SourceType m_demoType;
    QTimer m_demoTransactionTimer;
};

}
#endif // CSOURCEDEVICE_H
