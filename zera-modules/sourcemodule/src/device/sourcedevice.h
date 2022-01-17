#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "sourcedevicesubject.h"
#include "supportedsources.h"
#include "io-worker/ioworker.h"

#include <QObject>

/*
 * Start cmd worker on external demand
 * Notify busy state changed
 * Notify observers attached on command response
 * Keep source information
 * Allow adjustments on demo interfaces
 */
class SourceDevice : public SourceDeviceSubject
{
    Q_OBJECT
public:
    SourceDevice(tIoInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version);
    ~SourceDevice();

    // requests
    int startTransaction(const IoWorkerCmdPack &workerPack);
    void simulateExternalDisconnect();

    // setter
    void setDemoResponseDelay(bool followsTimeout, int fixedMs);

    // getter
    IoPacketGenerator getIoPacketGenerator();
    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
    QString getInterfaceInfo() const;
    bool isDemo() const;

signals:
    void sigBusyChanged(bool busy);
    void sigInterfaceDisconnected();

private slots:
    void onSourceCmdFinished(IoWorkerCmdPack cmdPack);

signals:
    void sigBusyChangedQueued(bool busy);

private:
    tIoInterfaceShPtr m_ioInterface;
    IoWorker m_sourceIoWorker;

    IoPacketGenerator m_outInGenerator;

    SupportedSourceTypes m_type;
    QString m_name;
    QString m_version;

    bool m_demoDelayFollowsTimeout = false;
    int m_demoDelayFixedMs = 0;
};

#endif // SOURCEDEVICE_H
