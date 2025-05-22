#ifndef ABSTRACTSOURCEIO_H
#define ABSTRACTSOURCEIO_H

#include "sourceproperties.h"
#include "ioqueuegroup.h"
#include <QObject>

class AbstractSourceIo : public QObject
{
    Q_OBJECT
public:
    virtual int startTransaction(IoQueueGroup::Ptr transferGroup) = 0;
    virtual SourceProperties getProperties() const = 0;
signals:
    void sigResponseReceived(const IoQueueGroup::Ptr response);
};

typedef std::shared_ptr<AbstractSourceIo> AbstractSourceIoPtr;

#endif // ABSTRACTSOURCEIO_H
