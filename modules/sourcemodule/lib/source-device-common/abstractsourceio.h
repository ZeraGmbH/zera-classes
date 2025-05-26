#ifndef ABSTRACTSOURCEIO_H
#define ABSTRACTSOURCEIO_H

#include "ioqueuegroup.h"
#include <QObject>
#include <QJsonObject>

class AbstractSourceIo : public QObject
{
    Q_OBJECT
public:
    virtual int startTransaction(IoQueueGroup::Ptr transferGroup) = 0;
    virtual QJsonObject getCapabilities() const = 0;
signals:
    void sigResponseReceived(const IoQueueGroup::Ptr response);
};

typedef std::shared_ptr<AbstractSourceIo> AbstractSourceIoPtr;

#endif // ABSTRACTSOURCEIO_H
