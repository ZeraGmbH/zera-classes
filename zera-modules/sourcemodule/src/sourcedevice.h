#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include "sourceinterface.h"
#include "sourcedevicestatus.h"
#include "sourceiopacketgenerator.h"
#include "supportedsources.h"

class cSourceInterfaceBase;

namespace SOURCEMODULE
{
class cSourceVeinInterface;

class cSourceDevice : public QObject
{
    Q_OBJECT
public:
    explicit cSourceDevice(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString version);
    virtual ~cSourceDevice();

    // requests
    void close();

    // getter
    tSourceInterfaceShPtr getIoInterface();
    bool isDemo();

    // setter
    void setVeinInterface(cSourceVeinInterface* veinInterface);
    void startActions(cSourceCommandPacket commandPack);
    void saveState(); // persistency

signals:
    void sigClosed(cSourceDevice* getSourceDevice);

public slots:
    void onNewVeinParamStatus(QVariant paramState);
    void onDemoOnOffFinished();

private slots:
    void onInterfaceClosed();

private:
    tSourceInterfaceShPtr m_ioInterface;

    cSourceJsonStateIo* m_paramStateLoadSave;
    cSourceJsonParamApi m_paramsRequested;
    cSourceJsonParamApi m_paramsCurrent;

    cSourceIoPacketGenerator* m_outInGenerator = nullptr;

    cSourceDeviceStatus  m_deviceStatus;
    cSourceVeinInterface* m_veinInterface = nullptr;

    SupportedSourceTypes m_type;
    QString m_version;

    QTimer m_demoOnOffDelayTimer;
};

}
#endif // CSOURCEDEVICE_H
