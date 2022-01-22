#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "sourcedevicesubject.h"
#include "supportedsources.h"
#include "sourceproperties.h"
#include "source-protocols/iogroupgenerator.h"
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
    SourceDevice(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties);
    ~SourceDevice();
    typedef QSharedPointer<SourceDevice> Ptr;

    // requests
    virtual int startTransaction(IoTransferDataGroup transferGroup);
    void simulateExternalDisconnect();

    // getter
    IoGroupGenerator getIoGroupGenerator();
    bool isIoBusy() const;
    QString getInterfaceInfo() const;
    SourceProperties getProperties() const;

signals:
    void sigSwitchTransationStarted();
    void sigInterfaceDisconnected();

private slots:
    void onIoGroupFinished(IoTransferDataGroup transferGroup);

signals:
    void sigSwitchTransationStartedQueued();

protected:

private:
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;

    IoGroupGenerator m_ioGroupGenerator;

    SourceProperties m_sourceProperties;
};

#endif // SOURCEDEVICE_H
