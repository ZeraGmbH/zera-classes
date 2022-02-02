#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "supportedsources.h"
#include "sourceproperties.h"
#include "source-protocols/iogroupgenerator.h"
#include "io-queue/ioqueue.h"

#include <QObject>

class ISourceIo : public QObject
{
    Q_OBJECT
public:
    virtual int startTransaction(IoQueueEntry::Ptr transferGroup) = 0;
    virtual IoGroupGenerator getIoGroupGenerator() const = 0;
    virtual SourceProperties getProperties() const = 0;
signals:
    void sigResponseReceived(const IoQueueEntry::Ptr response);
};


/*
 * Obtain group generator
 * Start multiple queued transactions
 * Notify observers attached on group response
 * Notify observers on io-device removed
 * Keep source properties
 */
class SourceIo : public ISourceIo
{
    Q_OBJECT
public:
    SourceIo(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties);
    ~SourceIo();

    // requests
    int startTransaction(IoQueueEntry::Ptr transferGroup) override;
    void simulateExternalDisconnect();

    // getter
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

signals:
    void sigIoDeviceDisconnected();

private slots:
    void onIoGroupFinished(IoQueueEntry::Ptr transferGroup);

private:
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;

    IoGroupGenerator m_ioGroupGenerator;

    SourceProperties m_sourceProperties;
};

#endif // SOURCEDEVICE_H
