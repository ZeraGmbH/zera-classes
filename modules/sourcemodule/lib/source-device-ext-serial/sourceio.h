#ifndef SOURCEDEVICE_H
#define SOURCEDEVICE_H

#include "sourceproperties.h"
#include "iogroupgenerator.h"
#include "ioqueue.h"
#include <QObject>

/*
 * Obtain group generator
 * Start multiple queued transactions
 * Notify observers attached on group response
 * Notify observers on io-device removed
 * Keep source properties
 */
class ISourceIo : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<ISourceIo> Ptr;
    virtual int startTransaction(IoQueueGroup::Ptr transferGroup) = 0;
    virtual IoGroupGenerator getIoGroupGenerator() const = 0;
    virtual SourceProperties getProperties() const = 0;
signals:
    void sigResponseReceived(const IoQueueGroup::Ptr response);
};

class SourceIo : public ISourceIo
{
    Q_OBJECT
public:
    SourceIo(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties);
    int startTransaction(IoQueueGroup::Ptr transferGroup) override;
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

private slots:
    void onIoGroupFinished(IoQueueGroup::Ptr transferGroup);
private:
    IoDeviceBase::Ptr m_ioDevice;
    IoQueue m_ioQueue;
    IoGroupGenerator m_ioGroupGenerator;
    SourceProperties m_sourceProperties;
};

#endif // SOURCEDEVICE_H
