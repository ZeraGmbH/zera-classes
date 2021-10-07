#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <zera-json-params-structure.h>
#include "sourcedevicestatus.h"

class cIOInterface;

namespace SOURCEMODULE
{
class cSourceVeinInterface;

class cSourceDevice : public QObject
{
    Q_OBJECT
public:
    enum SourceType { // TODO replace by protocol
        SOURCE_DEMO = 0,
        SOURCE_MT3000,
        SOURCE_MT400_20,
        SOURCE_MT_SINGLE_PHASE,

        SOURCE_DEMO_FG_4PHASE,

        SOURCE_TYPE_COUNT
    };
    explicit cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent = nullptr);
    virtual ~cSourceDevice();

    // requests
    void close();

    // getter
    cIOInterface* ioInterface();

    // setter
    void setVeinInterface(cSourceVeinInterface* veinInterface);

signals:
    void sigClosed(cSourceDevice* sourceDevice);

public slots:
    void newVeinParamStatus(QVariant paramState);
    void timeoutDemoTransaction();

private slots:
    void onInterfaceClosed(cIOInterface *ioInterface);

private:
    QString deviceFileName();
    const QJsonObject deviceParamInfo();
    const QJsonObject deviceParamState();

    cIOInterface* m_IOInterface = nullptr; // WE own the interface
    cZeraJsonParamsStructure m_ZeraJsonParamsStructure;
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
