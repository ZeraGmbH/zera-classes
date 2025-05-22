#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "abstractsourceio.h"
#include "ioqueue.h"

/*
 * Obtain group generator
 * Start multiple queued transactions
 * Notify observers attached on group response
 * Notify observers on io-device removed
 * Keep source properties
 */

class SourceIoExtSerial : public AbstractSourceIo
{
    Q_OBJECT
public:
    SourceIoExtSerial(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties);
    int startTransaction(IoQueueGroup::Ptr transferGroup) override;
    SourceProperties getProperties() const override;

private slots:
    void onIoGroupFinished(IoQueueGroup::Ptr transferGroup);
private:
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;
    SourceProperties m_sourceProperties;
};

#endif // SOURCEDEVICE_H
