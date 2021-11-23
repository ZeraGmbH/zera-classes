#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include "sourceinterface.h"
#include "sourcedevicestatus.h"
#include "sourceiopacketgenerator.h"
#include "supportedsources.h"
#include "sourceioworker.h"

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
    void saveState(); // persistency

signals:
    void sigClosed(cSourceDevice* getSourceDevice);

public slots:
    void onNewVeinParamStatus(QVariant paramState);
    void onSourceCmdFinished(cWorkerCommandPacket cmdPack);

private slots:
    void onInterfaceClosed();

private:
    tSourceInterfaceShPtr m_ioInterface;

    cSourceJsonStateIo* m_paramStateLoadSave;
    cSourceJsonParamApi m_paramsRequested;
    cSourceJsonParamApi m_paramsCurrent;

    cSourceIoPacketGenerator* m_outInGenerator = nullptr;
    cSourceIoWorker m_sourceIoWorker;
    int m_currentWorkerID = 0;

    cSourceDeviceStatus  m_deviceStatus;
    cSourceVeinInterface* m_veinInterface = nullptr;

    SupportedSourceTypes m_type;
    QString m_version;
};

}
#endif // CSOURCEDEVICE_H
