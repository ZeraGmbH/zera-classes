#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "sourcedevicesubject.h"
#include "supportedsources.h"
#include "io-queue/ioqueue.h"

#include <QObject>

/*
 * Start cmd queue on external demand
 * Notify busy state changed
 * Notify observers attached on command response
 * Keep source information
 */
class SourceDevice : public SourceDeviceSubject
{
    Q_OBJECT
public:
    SourceDevice(tIoDeviceShPtr interface, SupportedSourceTypes type, QString name, QString version);
    ~SourceDevice();

    // requests
    int startTransaction(IoTransferDataGroup transferGroup);
    void simulateExternalDisconnect();

    // setter
    void setDemoResponseDelay(bool followsTimeout, int fixedMs);
    void setDemoResonseError(bool active);

    // getter
    IoGroupGenerator getIoGroupGenerator();
    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
    QString getInterfaceInfo() const;
    bool isIoBusy() const;
    bool isDemo() const;

signals:
    void sigSwitchTransationStarted();
    void sigInterfaceDisconnected();

private slots:
    void onIoGroupFinished(IoTransferDataGroup transferGroup);

signals:
    void sigSwitchTransationStartedQueued();

protected:

private:
    void doDemoTransactionAdjustments(const IoTransferDataGroup &transferGroup);

    tIoDeviceShPtr m_ioInterface;
    IoQueue m_ioQueue;

    IoGroupGenerator m_ioGroupGenerator;

    SupportedSourceTypes m_type;
    QString m_name;
    QString m_version;

    bool m_demoDelayFollowsTimeout = false;
    int m_demoDelayFixedMs = 0;
    bool m_demoSimulErrorActive = false;
};

#endif // SOURCEDEVICE_H
